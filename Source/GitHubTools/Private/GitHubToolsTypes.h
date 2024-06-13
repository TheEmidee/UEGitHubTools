#pragma once

#include <CoreMinimal.h>

enum class EGitHubToolsFileState : uint8
{
    Unknown,
    Added,
    Modified,
    Removed,
    Renamed
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
    FGithubToolsPullRequestFileInfos( const FString & file_name, const FString & status );

    FString FileName;
    EGitHubToolsFileState FileState;
    FText AssetName;
    FText PackageName;
    FSlateIcon Icon;
    FName IconName;
    FText ToolTip;
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