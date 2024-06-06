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

struct FGithubToolsPullRequestComment
{
    FGithubToolsPullRequestComment() = default;
    FGithubToolsPullRequestComment( const FString & file_name, const FString & author, const FString & date );

    FText AssetName;
    FText Author;
    FText Date;
};

typedef TSharedPtr< FGithubToolsPullRequestComment > FGithubToolsPullRequestCommentPtr;