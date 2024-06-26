#pragma once

#include <CoreMinimal.h>

enum class EGitHubToolsPullRequestReviewEvent : uint8
{
    Approve,
    Comment,
    Dismiss,
    RequestChanges
};

enum class EGitHubToolsFileChangedState : uint8
{
    Unknown,
    Added,
    Modified,
    Removed,
    Renamed
};

enum class EGitHubToolsFileViewedState : uint8
{
    Unknown,
    Viewed,
    Unviewed,
    Dismissed
};

enum class EGitHubToolsCommitStatusState : uint8
{
    Error,
    Failure,
    Expected,
    Pending,
    Success,
    Unknown
};

struct FGithubToolsPullRequestComment
{
    FGithubToolsPullRequestComment() = default;
    explicit FGithubToolsPullRequestComment( const TSharedRef< FJsonObject > & json_object );

    FString Id;
    FText Author;
    FText Date;
    FText Comment;
    FString Path;
};

typedef TSharedPtr< FGithubToolsPullRequestComment > FGithubToolsPullRequestCommentPtr;

struct FGithubToolsPullRequestFileInfos
{
    FGithubToolsPullRequestFileInfos() = default;
    FGithubToolsPullRequestFileInfos( const FString & path, const FString & change_type, const FString & viewed_state );

    FString Path;
    FText AssetName;
    FText PackageName;
    EGitHubToolsFileChangedState ChangedState;
    FSlateIcon ChangedStateIcon;
    FName ChangedStateIconName;
    FText ChangedStateToolTip;
    EGitHubToolsFileViewedState ViewedState;
    FSlateIcon ViewedStateIcon;
    FName ViewedStateIconName;
    FText ViewedStateToolTip;
};

typedef TSharedPtr< FGithubToolsPullRequestFileInfos > FGithubToolsPullRequestFileInfosPtr;

enum class EGitHubToolsReviewState : uint8
{
    ChangesRequested,
    Comment,
    Unknown
};

struct FGithubToolsPullRequestReviewThreadInfos
{
    FGithubToolsPullRequestReviewThreadInfos() = default;
    explicit FGithubToolsPullRequestReviewThreadInfos( const TSharedRef< FJsonObject > & json_object );

    FString Id;
    bool bIsResolved;
    FString ResolvedByUserName;
    FString FileName;
    TArray< FGithubToolsPullRequestCommentPtr > Comments;
    int PRNumber;
};

typedef TSharedPtr< FGithubToolsPullRequestReviewThreadInfos > FGithubToolsPullRequestReviewThreadInfosPtr;

struct FGithubToolsPullRequestPendingReviewInfos
{
    FGithubToolsPullRequestPendingReviewInfos() = default;

    FString Id;
    TArray< FGithubToolsPullRequestCommentPtr > Comments;
};

typedef TSharedPtr< FGithubToolsPullRequestPendingReviewInfos > FGithubToolsPullRequestPendingReviewInfosPtr;

struct FGitHubToolsPullRequestCheckInfos
{
    FGitHubToolsPullRequestCheckInfos() = default;
    explicit FGitHubToolsPullRequestCheckInfos( const TSharedRef< FJsonObject > & json );

    FString Context;
    FString StateStr;
    FString Description;
    EGitHubToolsCommitStatusState State;
};

typedef TSharedPtr< FGitHubToolsPullRequestCheckInfos > FGitHubToolsPullRequestCheckInfosPtr;

struct FGithubToolsPullRequestInfos
{
    FGithubToolsPullRequestInfos() = default;
    explicit FGithubToolsPullRequestInfos( const TSharedRef< FJsonObject > & json );

    bool HasPendingReviews() const;

    FString ViewerLogin;
    int Number;
    FString Id;
    FString Title;
    FText Author;
    FString BaseRefName;
    FString Body;
    int ChangedFiles;
    int CommitCount;
    FString CreatedAt;
    FString HeadRefName;
    bool bIsDraft;
    bool bIsMergeable;
    FString State;
    FString URL;
    TArray< FGithubToolsPullRequestFileInfosPtr > FileInfos;
    TArray< FGithubToolsPullRequestReviewThreadInfosPtr > Reviews;
    TArray< FGitHubToolsPullRequestCheckInfosPtr > Checks;
    TArray< FGithubToolsPullRequestPendingReviewInfosPtr > PendingReviews;
};

FORCEINLINE bool FGithubToolsPullRequestInfos::HasPendingReviews() const
{
    return !PendingReviews.IsEmpty();
}

typedef TSharedPtr< FGithubToolsPullRequestInfos > FGithubToolsPullRequestInfosPtr;