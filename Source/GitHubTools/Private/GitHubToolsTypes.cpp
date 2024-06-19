#include "GitHubToolsTypes.h"

#include "RevisionControlStyle/RevisionControlStyle.h"

#define LOCTEXT_NAMESPACE "GitHubTools"
#define GET_ICON_RETURN( NAME ) FSlateIcon( FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl." #NAME )

namespace
{
    EGitHubToolsFileChangedState GetFileChangedState( const FString & status )
    {
        if ( status == TEXT( "ADDED" ) )
        {
            return EGitHubToolsFileChangedState::Added;
        }
        if ( status == TEXT( "MODIFIED" ) )
        {
            return EGitHubToolsFileChangedState::Modified;
        }
        if ( status == TEXT( "REMOVED" ) )
        {
            return EGitHubToolsFileChangedState::Removed;
        }
        if ( status == TEXT( "RENAMED" ) )
        {
            return EGitHubToolsFileChangedState::Renamed;
        }

        return EGitHubToolsFileChangedState::Unknown;
    }

    EGitHubToolsFileViewedState GetFileViewedState( const FString & status )
    {
        if ( status == TEXT( "DISMISSED" ) )
        {
            return EGitHubToolsFileViewedState::Dismissed;
        }
        if ( status == TEXT( "VIEWED" ) )
        {
            return EGitHubToolsFileViewedState::Viewed;
        }
        if ( status == TEXT( "UNVIEWED" ) )
        {
            return EGitHubToolsFileViewedState::Unviewed;
        }

        return EGitHubToolsFileViewedState::Unknown;
    }

    FSlateIcon GetSlateIconFromFileChangeState( EGitHubToolsFileChangedState file_state )
    {
        switch ( file_state )
        {
            case EGitHubToolsFileChangedState::Added:
                return GET_ICON_RETURN( OpenForAdd );
            case EGitHubToolsFileChangedState::Modified:
                return GET_ICON_RETURN( CheckedOut );
            case EGitHubToolsFileChangedState::Removed:
                return GET_ICON_RETURN( MarkedForDelete );
            case EGitHubToolsFileChangedState::Renamed:
                return GET_ICON_RETURN( CheckedOut );
            default:
                return FSlateIcon();
        }
    }

    FSlateIcon GetSlateIconFromFileViewedState( EGitHubToolsFileViewedState viewed_state )
    {
        switch ( viewed_state )
        {
            case EGitHubToolsFileViewedState::Dismissed:
                return GET_ICON_RETURN( OpenForAdd );
            case EGitHubToolsFileViewedState::Viewed:
                return GET_ICON_RETURN( CheckedOut );
            case EGitHubToolsFileViewedState::Unviewed:
                return GET_ICON_RETURN( MarkedForDelete );
            default:
                return FSlateIcon();
        }
    }

    FText GetToolTipFromFileChangedState( EGitHubToolsFileChangedState state )
    {
        switch ( state )
        {
            case EGitHubToolsFileChangedState::Added:
                return LOCTEXT( "FileAdded", "File Added" );
            case EGitHubToolsFileChangedState::Modified:
                return LOCTEXT( "FileModified", "File Modified" );
            case EGitHubToolsFileChangedState::Removed:
                return LOCTEXT( "FileRemoved", "File Removed" );
            case EGitHubToolsFileChangedState::Renamed:
                return LOCTEXT( "FileRenamed", "File Renamed" );
            default:
                return LOCTEXT( "FileRenamed", "Unknown file state" );
        }
    }

    FText GetToolTipFromFileViewedState( EGitHubToolsFileViewedState state )
    {
        switch ( state )
        {
            case EGitHubToolsFileViewedState::Dismissed:
                return LOCTEXT( "FileAdded", "No changes since last viewed" );
            case EGitHubToolsFileViewedState::Viewed:
                return LOCTEXT( "FileModified", "Viewed" );
            case EGitHubToolsFileViewedState::Unviewed:
                return LOCTEXT( "FileRemoved", "Unviewed" );
            default:
                return LOCTEXT( "FileRenamed", "Unknown viewed state" );
        }
    }

    EGitHubToolsReviewState GetReviewState( const FString & state )
    {
        if ( state == TEXT( "CHANGES_REQUESTED" ) )
        {
            return EGitHubToolsReviewState::ChangesRequested;
        }
        if ( state == TEXT( "COMMENTED" ) )
        {
            return EGitHubToolsReviewState::Comment;
        }

        return EGitHubToolsReviewState::Unknown;
    }
}

FGithubToolsPullRequestFileInfos::FGithubToolsPullRequestFileInfos( const FString & path, const FString & change_type, const FString & viewed_state ) :
    Path( path ),
    AssetName( FText::FromString( FPaths::GetCleanFilename( path ) ) ),
    PackageName( FText::FromString( path ) ),
    ChangedState( GetFileChangedState( change_type ) ),
    ChangedStateIcon( GetSlateIconFromFileChangeState( ChangedState ) ),
    ChangedStateIconName( ChangedStateIcon.GetStyleName() ),
    ChangedStateToolTip( GetToolTipFromFileChangedState( ChangedState ) ),
    ViewedState( GetFileViewedState( viewed_state ) ),
    ViewedStateIcon( GetSlateIconFromFileViewedState( ViewedState ) ),
    ViewedStateIconName( ViewedStateIcon.GetStyleName() ),
    ViewedStateToolTip( GetToolTipFromFileViewedState( ViewedState ) )
{
}

FGitHubToolsPullRequestCheckInfos::FGitHubToolsPullRequestCheckInfos( const TSharedPtr< FJsonObject > & json )
{
    Context = json->GetStringField( TEXT( "context" ) );
    State = json->GetStringField( TEXT( "state" ) );
    Description = json->GetStringField( TEXT( "description" ) );
}

FGithubToolsPullRequestInfos::FGithubToolsPullRequestInfos( const TSharedPtr< FJsonObject > & json )
{
    const auto author_object = json->GetObjectField( TEXT( "author" ) );
    const auto commits_object = json->GetObjectField( TEXT( "commits" ) );

    Number = json->GetIntegerField( TEXT( "number" ) );
    Id = json->GetStringField( TEXT( "id" ) );
    Title = json->GetStringField( TEXT( "title" ) );
    Author = FText::FromString( author_object->GetStringField( TEXT( "login" ) ) );
    BaseRefName = json->GetStringField( TEXT( "baseRefName" ) );
    Body = json->GetStringField( TEXT( "bodyText" ) );
    ChangedFiles = json->GetIntegerField( TEXT( "changedFiles" ) );
    CommitCount = commits_object->GetIntegerField( TEXT( "totalCount" ) );
    CreatedAt = json->GetStringField( TEXT( "createdAt" ) );
    HeadRefName = json->GetStringField( TEXT( "headRefName" ) );
    bIsDraft = json->GetBoolField( TEXT( "isDraft" ) );
    bIsMergeable = json->GetBoolField( TEXT( "mergeable" ) );
    URL = json->GetStringField( TEXT( "url" ) );
    State = json->GetStringField( TEXT( "state" ) );
}

#undef LOCTEXT_NAMESPACE
