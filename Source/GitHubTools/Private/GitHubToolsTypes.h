#pragma once

#include "CoreMinimal.h"

class FJsonObject;
struct FGithubToolsPullRequestReviewThreadInfos;
struct FGithubToolsPullRequestInfos;
struct FSlateBrush;

DECLARE_MULTICAST_DELEGATE( FGitHubToolsOnDataChanged );

enum class EGitHubToolsSubjectType : uint8
{
    File,
    Line,
    Unknown
};

enum class EGitHubToolsDiffSide : uint8
{
    Left,
    Right,
    Unknown
};

enum class EGitHubToolsPullRequestReviewEvent : uint8
{
    Approve,
    Comment,
    Dismiss,
    RequestChanges
};

enum class EGitHubToolsPullRequestReviewState : uint8
{
    Approved,
    ChangesRequested,
    Commented,
    Dismissed,
    Pending,
    Unknown
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
    UnViewed,
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

enum class EGitHubToolsPullRequestsState : uint8
{
    Closed,
    Merged,
    Open,
    Unknown
};

enum class EGitHubFileConversationStatus : uint8
{
    NoConversations,
    UnResolvedConversations,
    AllConversationsResolved
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

struct FGithubToolsPullRequestFilePatch
{
    FGithubToolsPullRequestFilePatch() = default;
    explicit FGithubToolsPullRequestFilePatch( const FString & file_name, const FString & patch );

    FString FileName;
    FString Patch;
};

typedef TSharedPtr< FGithubToolsPullRequestFilePatch > FGithubToolsPullRequestFilePatchPtr;

struct FGithubToolsPullRequestReviewThreadInfos
{
    FGithubToolsPullRequestReviewThreadInfos() = default;
    explicit FGithubToolsPullRequestReviewThreadInfos( const TSharedRef< FJsonObject > & json_object );

    FString Id;
    bool bIsResolved = false;
    FString ResolvedByUserName;
    FString FileName;
    EGitHubToolsDiffSide DiffSide;
    EGitHubToolsSubjectType SubjectType;
    int Line;
    TArray< FGithubToolsPullRequestCommentPtr > Comments;
    int PRNumber;
    TSharedPtr< struct FGithubToolsPullRequestFileInfos > ParentFileInfos;
    // Set to true when we add a comment on a file. It is set back to false when the PR is approved. When the PR is dismissed, it is used to find which threads to delete to stay in sync with github
    bool bIsPending = false;
};

typedef TSharedPtr< FGithubToolsPullRequestReviewThreadInfos > FGithubToolsPullRequestReviewThreadInfosPtr;

struct FGithubToolsPullRequestFileInfos : TSharedFromThis< FGithubToolsPullRequestFileInfos >
{
    FGithubToolsPullRequestFileInfos() = default;
    FGithubToolsPullRequestFileInfos( const FString & path, const FString & change_type, const FString & viewed_state );

    void UpdateViewedState( EGitHubToolsFileViewedState new_viewed_state );
    bool IsUAsset() const;
    bool IsFromDeveloperFolder() const;
    bool IsOFPAAsset() const;
    void AddReview( const FGithubToolsPullRequestReviewThreadInfosPtr & review_thread_infos );
    void RefreshResolvedConversations();
    void RemovePendingReviews();

    FGitHubToolsOnDataChanged OnDataChanged;

    FString Path;
    FText AssetName;
    FText PackageName;
    EGitHubToolsFileChangedState ChangedState;
    const FSlateBrush * ChangedStateBrush;
    FText ChangedStateToolTip;
    EGitHubToolsFileViewedState ViewedState;
    const FSlateBrush * ViewedStateBrush;
    FText ViewedStateToolTip;
    EGitHubFileConversationStatus ConversationStatus;
    FString Patch;
    TSharedPtr< FGithubToolsPullRequestInfos > PRInfos;
    TArray< TSharedPtr< FGithubToolsPullRequestReviewThreadInfos > > Reviews;
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
    explicit FGithubToolsPullRequestReviewInfos( const TSharedRef< FJsonObject > & json );

    FString Id;
    FString Author;
    EGitHubToolsPullRequestReviewState State;
    TArray< FGithubToolsPullRequestCommentPtr > Comments;
};

typedef TSharedPtr< FGithubToolsPullRequestReviewInfos > FGithubToolsPullRequestPendingReviewInfosPtr;

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

struct FGithubToolsPullRequestInfos : TSharedFromThis< FGithubToolsPullRequestInfos >
{
    FGithubToolsPullRequestInfos() = default;
    explicit FGithubToolsPullRequestInfos( const TSharedRef< FJsonObject > & json );

    bool CanCommentFiles() const;
    bool HasPendingReview() const;
    void SetFiles( const TArray< FGithubToolsPullRequestFileInfosPtr > & files, const TArray< FGithubToolsPullRequestFilePatchPtr > & patches, const TArray< FGithubToolsPullRequestReviewThreadInfosPtr > & reviews );
    bool CanApprovePullRequest() const;
    bool IsApprovedByMe() const;
    void DismissReview();
    void RequestChanges();
    void ApproveReview();

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
    bool bIsMerged;
    EGitHubToolsPullRequestsState State;
    FString URL;
    bool bHasUnresolvedConversations;
    TArray< FGithubToolsPullRequestFileInfosPtr > FileInfos;
    TArray< FGitHubToolsPullRequestCheckInfosPtr > Checks;
    TArray< FGithubToolsPullRequestCommentPtr > Comments;
    TArray< FGithubToolsPullRequestPendingReviewInfosPtr > Reviews;
    FGithubToolsPullRequestPendingReviewInfosPtr PendingReview;

private:
    void ClearPendingReview();
};

FORCEINLINE bool FGithubToolsPullRequestInfos::HasPendingReview() const
{
    return PendingReview != nullptr;
}

typedef TSharedPtr< FGithubToolsPullRequestInfos > FGithubToolsPullRequestInfosPtr;

struct FGitHubToolsOpenedPullRequestInfos
{
    FGitHubToolsOpenedPullRequestInfos() = default;
    explicit FGitHubToolsOpenedPullRequestInfos( const TSharedRef< FJsonObject > & json );

    int Number;
    FString Title;
    FText Author;
    FString HeadRefName;
    bool bIsCurrentPR = false;
};

typedef TSharedPtr< FGitHubToolsOpenedPullRequestInfos > FGitHubToolsOpenedPullRequestInfosPtr;

struct FGitHubToolsFileInfosTreeItem
{
    explicit FGitHubToolsFileInfosTreeItem( const FString & path ) :
        Path( path )
    {}

    FString Path;
    FGithubToolsPullRequestFileInfosPtr FileInfos;
    TArray< TSharedPtr< FGitHubToolsFileInfosTreeItem > > Children;
};

typedef TSharedPtr< FGitHubToolsFileInfosTreeItem > FGitHubToolsFileInfosTreeItemPtr;