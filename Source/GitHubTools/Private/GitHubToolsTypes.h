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

struct FGithubToolsPullRequestComment
{
    FGithubToolsPullRequestComment() = default;

    FString Id;
    FText Author;
    FText Date;
    FText Comment;
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

    FString Id;
    bool bIsResolved;
    FText ResolvedByUserName;
    FString FileName;
    TArray< FGithubToolsPullRequestCommentPtr > Comments;
    int PRNumber;
};

typedef TSharedPtr< FGithubToolsPullRequestReviewThreadInfos > FGithubToolsPullRequestReviewThreadInfosPtr;

struct FGithubToolsPullRequestInfos
{
    FGithubToolsPullRequestInfos( int number, const FString & id, const FString & title );

    int Number;
    FString Id;
    FText Title;
    TArray< FGithubToolsPullRequestFileInfosPtr > FileInfos;
    TArray< FGithubToolsPullRequestReviewThreadInfosPtr > Reviews;
};

typedef TSharedPtr< FGithubToolsPullRequestInfos > FGithubToolsPullRequestInfosPtr;