# GIT CHEATSHEET


### Where is the git config file located

`git config --list --show-origin`

### Useful aliases for Windows bash prompt

`alias branch='git checkout '` <br/>
`alias cleanupStale='git remote prune origin'` <br/>

> _Deletes all stale remote-tracking branches under . These stale branches have already been removed from the remote repository referenced by , but are still locally available in "remotes/"._

`alias diffs='git difftool -y'` <br/>
`alias dir='ls -l'` <br/>
`alias filesincommit='git diff-tree --no-commit-id --name-only -r'` <br/>
`alias githelp='/c/Program\ Files/Mozilla\ Firefox/firefox.exe C:\\Sandbox\\panos_devtools\\DefensiveCoding\\scripts\\git\\git_cheatsheet.md'` <br/>
`alias gs='git status'` <br/>
`alias hist='git log --oneline -10'` <br/>
`alias listbranches='git branch --list'` <br/>
`alias makebranch='git checkout -b'` <br/>
`alias sync='git fetch && git pull'` <br/>

### shortcuts

#### fetch and prune refs to deleted branches

`git branch -vv | grep 'origin/.*: gone]' | awk '{print $1}' | xargs git branch -d  `

#### recent history with pretty formatting
 
`git log --abbrev-commit --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s %Cgreen(%cr) %C(bold blue)<%an>%Creset' -10`
 
#### recent history, wwithout submitter name
 
`git log --abbrev-commit --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s %Cgreen(%cr) %C(bold blue)%Creset' -10`

#### simplified log of 10 recent commits

`git log --abbrev-commit --pretty=format:'%Cred[%h]%Creset -%C(yellow)%d%Creset %s' -10`

### How to list all the files in a commit

_Preferred Way (because it's a plumbing command; meant to be programmatic)_

`git diff-tree --no-commit-id --name-only -r bd61ad98` <br/>

_Another Way (less preferred for scripts, because it's a porcelain command; meant to be user-facing)_

`git show --pretty="" --name-only bd61ad98` <br/>

### changing a commit message

https://help.github.com/articles/changing-a-commit-message/

_(useful, for instance if during a local commit you forget the JIRA key prefix as required by process guidelines)_

### 'social commit': how to create pull requests with commits that contain acknowledgement of contribution of other developers (e.g. in a process of extreme programming)

_commit like this:_

git commit -m "We fixed this thing" -- author 'Jennifer H. Pair <jenny.pair@example.com>'

_this way the author is different than the committer, so both names are contained in the commit, and shown on the ui (e.g. github code review)_
