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
        if ( status == TEXT( "DELETED" ) )
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

    EGitHubToolsCommitStatusState GetCommitState( const FString & state )
    {
        if ( state == TEXT( "ERROR" ) )
        {
            return EGitHubToolsCommitStatusState::Error;
        }
        if ( state == TEXT( "EXPECTED" ) )
        {
            return EGitHubToolsCommitStatusState::Expected;
        }
        if ( state == TEXT( "FAILURE" ) )
        {
            return EGitHubToolsCommitStatusState::Failure;
        }
        if ( state == TEXT( "PENDING" ) )
        {
            return EGitHubToolsCommitStatusState::Pending;
        }
        if ( state == TEXT( "SUCCESS" ) )
        {
            return EGitHubToolsCommitStatusState::Success;
        }

        return EGitHubToolsCommitStatusState::Unknown;
    }

    EGitHubToolsPullRequestsState GetPullRequestState( const FString & state )
    {
        if ( state == TEXT( "CLOSED" ) )
        {
            return EGitHubToolsPullRequestsState::Closed;
        }
        if ( state == TEXT( "MERGED" ) )
        {
            return EGitHubToolsPullRequestsState::Merged;
        }
        if ( state == TEXT( "OPEN" ) )
        {
            return EGitHubToolsPullRequestsState::Open;
        }

        return EGitHubToolsPullRequestsState::Unknown;
    }
}

FGithubToolsPullRequestComment::FGithubToolsPullRequestComment( const TSharedRef< FJsonObject > & json_object )
{
    Id = json_object->GetStringField( TEXT( "id" ) );

    const auto comment_author_object = json_object->GetObjectField( TEXT( "author" ) );
    Author = FText::FromString( comment_author_object->GetStringField( TEXT( "login" ) ) );
    Comment = FText::FromString( json_object->GetStringField( TEXT( "body" ) ) );
    Date = FText::FromString( json_object->GetStringField( TEXT( "createdAt" ) ) );

    if ( json_object->HasField( TEXT( "path" ) ) )
    {
        Path = json_object->GetStringField( TEXT( "path" ) );
    }
}

FGithubToolsPullRequestFileInfos::FGithubToolsPullRequestFileInfos( const FString & path, const FString & change_type, const FString & viewed_state ) :
    Path( path ),
    AssetName( FText::FromString( FPaths::GetCleanFilename( path ) ) ),
    PackageName( FText::FromString( path ) ),
    ChangedState( GetFileChangedState( change_type ) ),
    ChangedStateIcon( GetSlateIconFromFileChangeState( ChangedState ) ),
    ChangedStateIconName( ChangedStateIcon.GetStyleName() ),
    ChangedStateToolTip( GetToolTipFromFileChangedState( ChangedState ) )
{
    UpdateViewedState( GetFileViewedState( viewed_state ) );
}

void FGithubToolsPullRequestFileInfos::UpdateViewedState( EGitHubToolsFileViewedState new_viewed_state )
{
    ViewedState = new_viewed_state;
    ViewedStateIcon = GetSlateIconFromFileViewedState( ViewedState );
    ViewedStateIconName = ViewedStateIcon.GetStyleName();
    ViewedStateToolTip = GetToolTipFromFileViewedState( ViewedState );
}

bool FGithubToolsPullRequestFileInfos::IsUAsset() const
{
    return Path.EndsWith( TEXT( ".uasset" ) );
}

FGithubToolsPullRequestReviewThreadInfos::FGithubToolsPullRequestReviewThreadInfos( const TSharedRef< FJsonObject > & json_object ) :
    PRNumber( INDEX_NONE )
{
    Id = json_object->GetStringField( TEXT( "id" ) );
    bIsResolved = json_object->GetBoolField( TEXT( "isResolved" ) );
    FileName = json_object->GetStringField( TEXT( "path" ) );

    const auto get_resolved_by_user_name = [ &json_object ]() {
        FString user_name( TEXT( "" ) );
        if ( json_object->HasField( TEXT( "resolvedBy" ) ) )
        {
            const TSharedPtr< FJsonObject > * review_author_object;

            if ( json_object->TryGetObjectField( TEXT( "resolvedBy" ), review_author_object ) )
            {
                user_name = ( *review_author_object )->GetStringField( TEXT( "login" ) );
            }
        }

        return user_name;
    };

    ResolvedByUserName = get_resolved_by_user_name();
}

FGitHubToolsPullRequestCheckInfos::FGitHubToolsPullRequestCheckInfos( const TSharedRef< FJsonObject > & json )
{
    Context = json->GetStringField( TEXT( "context" ) );
    StateStr = json->GetStringField( TEXT( "state" ) );
    State = GetCommitState( StateStr );
    Description = json->GetStringField( TEXT( "description" ) );
}

FGithubToolsPullRequestInfos::FGithubToolsPullRequestInfos( const TSharedRef< FJsonObject > & json ):
    bApprovedByMe( false )
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
    State = GetPullRequestState( json->GetStringField( TEXT( "state" ) ) );
}

bool FGithubToolsPullRequestInfos::CanCommentFiles() const
{
    return !HasPendingReviews() && State == EGitHubToolsPullRequestsState::Open;
}

#undef LOCTEXT_NAMESPACE
