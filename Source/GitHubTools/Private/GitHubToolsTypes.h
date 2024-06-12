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
    FGithubToolsPullRequestComment( const FString & file_name, const FString & author, const FString & date, const FString & comment );

    FText AssetName;
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

struct FGithubToolsPullRequestReviewInfos
{
    FGithubToolsPullRequestReviewInfos() = default;
    FGithubToolsPullRequestReviewInfos( const FString & id, const FString & user_name, const FString & state, const FString & submitted_at );

    FString Id;
    FText UserName;
    FText SubmittedAt;
    EGitHubToolsReviewState State;
    TArray< FGithubToolsPullRequestCommentPtr > Comments;
};

typedef TSharedPtr< FGithubToolsPullRequestReviewInfos > FGithubToolsPullRequestReviewInfosPtr;

struct FGithubToolsPullRequestInfos
{
    FGithubToolsPullRequestInfos( int number, const FString & title );

    int Number;
    FText Title;
    TArray< FGithubToolsPullRequestFileInfosPtr > FileInfos;
    TArray< FGithubToolsPullRequestCommentPtr > Comments;
    TArray< FGithubToolsPullRequestReviewInfosPtr > Reviews;
};

typedef TSharedPtr< FGithubToolsPullRequestInfos > FGithubToolsPullRequestInfosPtr;