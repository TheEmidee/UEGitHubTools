# UE GitHub Tools

This plugin provides tools to interact with GitHub directly from the editor.

For now, you can manage pull requests.

You have the list of all the files that have been modified in the PR, you can open the files directly, or open the diff tool from UE.

You can also mark files as viewed, add comments to them, show the checks, approve the PR, etc...

## Setup

You must add the sources of this tool in the `Plugins` folder of your project. The easiest is to add the plugin as a submodule, using a command like `git submodule add git@github.com:TheEmidee/UEGitHubTools.git Plugins/GitHubTools`.

This plugin depends on [UEGitPlugin](https://github.com/ProjectBorealis/UEGitPlugin), which you can install the same way.

Then you must create a personal access token on GitHub for the plugin to be able to communicate with the GitHub API.

You can follow [this tutorial here](https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/managing-your-personal-access-tokens) to add a fine-grained personal access token.

The permissions you need to grant to the token are:

* Commit statuses (Read-only)
* Contents (Read and write)
* Metadata (Read-only)
* Pull requests (Read and write)

When you have generated the token, copy it and open the editor. Then open the `Editor Preferences`, click on the `GitHub Tools` section in `General`, and paste your token. You also need to fill the `Repository Owner` and the `Repository Name` settings.

## Open the PR Infos window

To use the plugin in the editor, after you have finished to configure the plugin and the git plugin, you can click on the bottom-right button `Revision Control` in the main window, then select the contextual menu `Open the review tool window`.

The plugin will fetch the PRs of the repository and display the informations of the PR that matches the branch that is checked out locally in your repository. From there you can view the general information of the PR, list the files, act on them, etc...