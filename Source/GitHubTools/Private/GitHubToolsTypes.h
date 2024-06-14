#pragma once

#include <CoreMinimal.h>

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
    FGithubToolsPullRequestFileInfos( const FString & file_name, const FString & change_type, const FString & viewed_state );

    FString FileName;
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
};

typedef TSharedPtr< FGithubToolsPullRequestReviewThreadInfos > FGithubToolsPullRequestReviewThreadInfosPtr;

struct FGithubToolsPullRequestInfos
{
    FGithubToolsPullRequestInfos( int number, const FString & title );

    int Number;
    FText Title;
    TArray< FGithubToolsPullRequestFileInfosPtr > FileInfos;
    TArray< FGithubToolsPullRequestReviewThreadInfosPtr > Reviews;
};

typedef TSharedPtr< FGithubToolsPullRequestInfos > FGithubToolsPullRequestInfosPtr;