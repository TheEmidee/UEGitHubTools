#include "GitHubToolsTypes.h"

#include "RevisionControlStyle/RevisionControlStyle.h"

#define LOCTEXT_NAMESPACE "GitHubTools"
#define GET_ICON_RETURN( NAME ) FSlateIcon( FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl." #NAME )

namespace
{
    EGitHubToolsFileState GetFileStateFromStatus( const FString & status )
    {
        if ( status == TEXT( "added" ) )
        {
            return EGitHubToolsFileState::Added;
        }
        if ( status == TEXT( "modified" ) )
        {
            return EGitHubToolsFileState::Modified;
        }
        if ( status == TEXT( "removed" ) )
        {
            return EGitHubToolsFileState::Removed;
        }
        if ( status == TEXT( "renamed" ) )
        {
            return EGitHubToolsFileState::Renamed;
        }

        return EGitHubToolsFileState::Unknown;
    }

    FSlateIcon GetSlateIconFromFileState( EGitHubToolsFileState file_state )
    {
        switch ( file_state )
        {
            case EGitHubToolsFileState::Added:
                return GET_ICON_RETURN( OpenForAdd );
            case EGitHubToolsFileState::Modified:
                return GET_ICON_RETURN( CheckedOut );
            case EGitHubToolsFileState::Removed:
                return GET_ICON_RETURN( MarkedForDelete );
            case EGitHubToolsFileState::Renamed:
                return GET_ICON_RETURN( CheckedOut );
            default:
                return FSlateIcon();
        }
    }

    FText GetToolTipFromFileState( EGitHubToolsFileState file_state )
    {
        switch ( file_state )
        {
            case EGitHubToolsFileState::Added:
                return LOCTEXT( "FileAdded", "File Added" );
            case EGitHubToolsFileState::Modified:
                return LOCTEXT( "FileModified", "File Modified" );
            case EGitHubToolsFileState::Removed:
                return LOCTEXT( "FileRemoved", "File Removed" );
            case EGitHubToolsFileState::Renamed:
                return LOCTEXT( "FileRenamed", "File Renamed" );
            default:
                return LOCTEXT( "FileRenamed", "Unknown file state" );
        }
    }
}

FGithubToolsPullRequestFileInfos::FGithubToolsPullRequestFileInfos( const FString & file_name, const FString & status ) :
    FileName( file_name ),
    FileState( GetFileStateFromStatus( status ) ),
    AssetName( FText::FromString( FPaths::GetCleanFilename( file_name ) ) ),
    PackageName( FText::FromString( file_name ) ),
    Icon( GetSlateIconFromFileState( FileState ) ),
    IconName( Icon.GetStyleName() ),
    ToolTip( GetToolTipFromFileState( FileState ) )
{
}

FGithubToolsPullRequestComment::FGithubToolsPullRequestComment( const FString & file_name, const FString & author, const FString & date ) :
    AssetName( FText::FromString( FPaths::GetCleanFilename( file_name ) ) ),
    Author( FText::FromString( FPaths::GetCleanFilename( author ) ) ),
    Date( FText::FromString( FPaths::GetCleanFilename( date ) ) )
{
}

#undef LOCTEXT_NAMESPACE
