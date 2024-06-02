#include "GitHubToolsReviewFileItem.h"

#include <ActorFolder.h>
#include <ActorFolderDesc.h>
#include <Algo/Count.h>
#include <AssetToolsModule.h>
#include <ISourceControlModule.h>

#if SOURCE_CONTROL_WITH_SLATE

#define LOCTEXT_NAMESPACE "GitHubToolsReviewFileItem"

namespace
{
    FText GetDefaultAssetName()
    {
        return LOCTEXT( "SourceControl_DefaultAssetName", "Unavailable" );
    }

    FText GetDefaultAssetType()
    {
        return LOCTEXT( "SourceControl_DefaultAssetType", "Unknown" );
    }

    FText GetDefaultUnknownAssetType()
    {
        return LOCTEXT( "SourceControl_FileTypeDefault", "{0} File" );
    }

    FText GetDefaultMultipleAsset()
    {
        return LOCTEXT( "SourceCOntrol_ManyAssetType", "Multiple Assets" );
    }

    FString RetrieveAssetName( const FAssetData & InAssetData )
    {
        static const FName NAME_ActorLabel( TEXT( "ActorLabel" ) );

        if ( InAssetData.FindTag( NAME_ActorLabel ) )
        {
            FString ResultAssetName = TEXT( "" );

            InAssetData.GetTagValue( NAME_ActorLabel, ResultAssetName );
            return ResultAssetName;
        }
        else if ( InAssetData.AssetClassPath == UActorFolder::StaticClass()->GetClassPathName() )
        {
            FString ActorFolderPath = UActorFolder::GetAssetRegistryInfoFromPackage( InAssetData.PackageName ).GetDisplayName();
            if ( !ActorFolderPath.IsEmpty() )
            {
                return ActorFolderPath;
            }
        }

        return InAssetData.AssetName.ToString();
    }

    FString RetrieveAssetPath( const FAssetData & InAssetData )
    {
        int32 LastDot = -1;
        FString Path = InAssetData.GetObjectPathString();

        // Strip asset name from object path
        if ( Path.FindLastChar( '.', LastDot ) )
        {
            Path.LeftInline( LastDot );
        }

        return Path;
    }

    void RefreshAssetInformationInternal( const TArray< FAssetData > & Assets, const FString & InFilename, FString & OutAssetName, FString & OutAssetPath, FString & OutAssetType, FText & OutPackageName, FColor & OutAssetTypeColor )
    {
        // Initialize display-related members
        FString Filename = InFilename;
        FString TempAssetName = GetDefaultAssetName().ToString();
        FString TempAssetPath = Filename;
        FString TempAssetType = GetDefaultAssetType().ToString();
        FString TempPackageName = Filename;
        FColor TempAssetColor = FColor( // Copied from ContentBrowserCLR.cpp
            127 + FColor::Red.R / 2,    // Desaturate the colors a bit (GB colors were too.. much)
            127 + FColor::Red.G / 2,
            127 + FColor::Red.B / 2,
            200 ); // Opacity

        if ( Assets.Num() > 0 )
        {
            auto IsNotRedirector = []( const FAssetData & InAssetData ) {
                return !InAssetData.IsRedirector();
            };
            int32 NumUserFacingAsset = Algo::CountIf( Assets, IsNotRedirector );

            if ( NumUserFacingAsset == 1 )
            {
                const FAssetData & AssetData = *Algo::FindByPredicate( Assets, IsNotRedirector );

                TempAssetName = RetrieveAssetName( AssetData );
                TempAssetPath = RetrieveAssetPath( AssetData );
                TempAssetType = AssetData.AssetClassPath.ToString();

                const FAssetToolsModule & AssetToolsModule = FModuleManager::LoadModuleChecked< FAssetToolsModule >( TEXT( "AssetTools" ) );
                const TSharedPtr< IAssetTypeActions > AssetTypeActions = AssetToolsModule.Get().GetAssetTypeActionsForClass( AssetData.GetClass() ).Pin();

                if ( AssetTypeActions.IsValid() )
                {
                    TempAssetColor = AssetTypeActions->GetTypeColor();
                }
                else
                {
                    TempAssetColor = FColor::White;
                }
            }
            else
            {
                TempAssetName = RetrieveAssetName( Assets[ 0 ] );
                TempAssetPath = RetrieveAssetPath( Assets[ 0 ] );

                for ( int32 i = 1; i < Assets.Num(); ++i )
                {
                    TempAssetName += TEXT( ";" ) + RetrieveAssetName( Assets[ i ] );
                }

                TempAssetType = GetDefaultMultipleAsset().ToString();
                TempAssetColor = FColor::White;
            }

            // Beautify the package name
            TempPackageName = TempAssetPath + "." + TempAssetName;
        }
        else if ( FPackageName::TryConvertFilenameToLongPackageName( Filename, TempPackageName ) )
        {
            // Fake asset name, asset path from the package name
            TempAssetPath = TempPackageName;

            int32 LastSlash = -1;
            if ( TempPackageName.FindLastChar( '/', LastSlash ) )
            {
                TempAssetName = TempPackageName;
                TempAssetName.RightChopInline( LastSlash + 1 );
            }
        }
        else
        {
            TempAssetName = FPaths::GetCleanFilename( Filename );
            TempPackageName = Filename; // put back original package name if the try failed
            TempAssetType = FText::Format( GetDefaultUnknownAssetType(), FText::FromString( FPaths::GetExtension( Filename ).ToUpper() ) ).ToString();
        }

        // Finally, assign the temp variables to the member variables
        OutAssetName = TempAssetName;
        OutAssetPath = TempAssetPath;
        OutAssetType = TempAssetType;
        OutAssetTypeColor = TempAssetColor;
        OutPackageName = FText::FromString( TempPackageName );
    }
}

FGitSourceControlReviewFileItem::FGitSourceControlReviewFileItem( const TSharedRef< FGitSourceControlState > & file_state ) :
    FileState( file_state ),
    CheckBoxState( file_state->IsDeleted() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked )
{
    ISourceControlModule::Get().GetAssetDataCache().GetAssetDataArray( FileState, Assets );

    RefreshAssetInformation();
}

void FGitSourceControlReviewFileItem::RefreshAssetInformation()
{
    static TArray< FAssetData > no_assets;
    RefreshAssetInformationInternal( Assets.IsValid() ? *Assets : no_assets, FileState->GetFilename(), AssetNameStr, AssetPathStr, AssetTypeStr, PackageName, AssetTypeColor );

    AssetName = FText::FromString( AssetNameStr );
    AssetPath = FText::FromString( AssetPathStr );
    AssetType = FText::FromString( AssetTypeStr );
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE
