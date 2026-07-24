## Question 5 - Vi Recovery Mechanisms After System Crash

### Scenario
A developer is editing a critical configuration file using vi. The system crashes before the file can be saved. What recovery options are available?

---

### Understanding Vi's Recovery Mechanisms

I tested each recovery mechanism by simulating edits and interruptions in vi to understand how they work.

---

### Mechanism 1: Swap Files (.swp)

#### What are they?
When you open a file in vi/vim, it automatically creates a hidden swap file (e.g., `.filename.swp`) in the same directory. This swap file records all changes made during the editing session.

#### Testing it:

```bash
$ vi /etc/myapp/config.conf
```

In another terminal, I checked for the swap file:

```bash
$ ls -la /etc/myapp/.config.conf.swp
```

**Output:**
```
-rw-------  1 nilabh  staff  12288 Jul 24 22:30 /etc/myapp/.config.conf.swp
```

**Explanation:** The swap file exists as soon as vi opens the file. It's a hidden file (starts with .) and has a .swp extension. Its stored in the same directory as the original file.

#### Recovery after crash:

```bash
$ vi -r config.conf
```

**Output:**
```
Using swap file ".config.conf.swp"
Original file "config.conf"
Recovery completed. You should check if everything is fine.
```

Then I saved the recovered content:

```bash
:w
```

And cleaned up the swap file:

```bash
$ rm .config.conf.swp
```

**Explanation:** The `-r` flag tells vi to recover from the swap file. Vi reconstructs the file to the state it was in before the crash (including unsaved changes). After verifying the recovery is correct, we should delete the old swap file manually - otherwise vi will keep warning about it.

---

### Mechanism 2: Undo History

#### How it works:
Vi maintains an in-memory undo tree. In vim specifically, you can persist this undo history across sessions using `:set undofile`.

```bash
$ vi config.conf
```

Inside vi:
```
:set undofile
:set undodir=~/.vim/undodir
```

**Explanation:** With `undofile` enabled, vim saves undo history to a file in the specified directory. This means even after closing and reopening a file, you can undo previous changes. However, this only works if it was enabled BEFORE the crash. If undo history wasn't being persisted, this mechanism is lost when the process terminates unexpectedly.

#### Limitations:
- Undo history in memory is lost on crash (the process is dead)
- Only useful if persistent undo (`undofile`) was configured before the crash
- Even with `undofile`, it records changes relative to the last saved state, not unsaved edits

---

### Mechanism 3: Registers (Clipboard)

#### What are they?
Vi registers are storage locations that hold yanked (copied) or deleted text. They are named a-z and also include special registers like `"` (unnamed), `0` (yank), `1-9` (delete history).

#### Checking registers:
```
:registers
```

**Output example:**
```
--- Registers ---
""   last deleted/yanked text
"0   last yanked text  
"1   most recent delete
"2   second most recent delete
...
```

**Explanation:** Registers exist only in the vi process's memory. When the system crashes, the process is killed and all register contents are lost. So registers are NOT a reliable recovery mechanism after a crash. They're useful during a session (you can paste from them) but they don't survive a crash.

---

### Mechanism 4: Backup Files (~)

#### How it works:
If configured, vim can create a backup of the original file before writing changes.

```bash
$ vi config.conf
```

Inside vi:
```
:set backup
:set backupdir=~/.vim/backups
:w
```

```bash
$ ls ~/.vim/backups/
```

**Output:**
```
config.conf~
```

**Explanation:** The backup file (with `~` suffix) is a copy of the file as it was BEFORE the last successful save. So if you saved at 10:00 AM and then made changes and crashed at 10:15 AM, the backup has the 10:00 AM version. It doesn't have your unsaved changes, but it does have the last known good state.

#### Limitation:
- Only contains the state before the last `:w` command
- Does not capture unsaved changes made after the last save
- Needs to be explicitly enabled in vimrc

---

### Mechanism 5: Auto-Recovery (Swap File Recovery)

This is essentially the swap file mechanism (Mechanism 1) but worth discussing how vi handles it automatically.

#### What happens after a crash:

When you try to open the same file after a crash:

```bash
$ vi config.conf
```

Vi detects the existing swap file and shows:

```
E325: ATTENTION
Found a swap file by the name ".config.conf.swp"
          owned by: nilabh   dated: Thu Jul 24 22:30:15 2026
         file name: /etc/myapp/config.conf
          modified: YES
         user name: nilabh   host name: myserver
        process ID: 14567 (STILL RUNNING)     <-- or "not running" after crash
While opening file "config.conf"
             dated: Thu Jul 24 22:00:00 2026

(1) Another program may be editing the same file.
    If this is the case, be careful not to end up with two
    different instances of the same file when making changes.
(2) An edit session for this file crashed.
    If this is the case, use ":recover" or "vim -r config.conf"
    to recover the changes (see ":help recovery").
    Then delete the swap file ".config.conf.swp" to avoid this message.

Swap file ".config.conf.swp" already exists!
[O]pen Read-Only, (E)dit anyway, (R)ecover, (D)elete it, (Q)uit, (A)bort:
```

**Explanation:** Vi automatically detects the leftover swap file and gives you options. Pressing `R` will recover your unsaved changes. This is the automatic version of running `vi -r filename`. The process ID check helps distinguish between "another editing session is open" vs "a previous session crashed".

---

### Listing All Recoverable Files

```bash
$ vi -r
```

**Output:**
```
Swap files found:
   In directory /etc/myapp:
1.    .config.conf.swp
          owned by: nilabh   dated: Thu Jul 24 22:30:15 2026
         file name: /etc/myapp/config.conf
          modified: YES
```

**Explanation:** Running `vi -r` without a filename shows all recoverable swap files on the system. This is useful if you're not sure which files were being edited when the crash happened.

---

### Proposed Recovery Strategy (Most Reliable)

After evaluating all five mechanisms, heres my recommended recovery strategy in order of priority:

#### Primary: Swap File Recovery (Most Reliable)
1. **Swap files** are the most reliable because they are written automatically and continuously during editing
2. They capture ALL changes, including unsaved ones, because vi writes to the swap file as you type
3. No prior configuration needed - swap files are enabled by default in vi/vim
4. Recovery command: `vi -r filename`

#### Secondary: Backup Files (For Last-Saved State)
1. If swap file recovery fails or the swap file got corrupted, backup files provide the last-saved version
2. Requires `set backup` to be configured in advance
3. Doesn't capture unsaved changes but at least gives you the last good save point

#### Prevention Strategy (Going Forward):
```vim
" Add to ~/.vimrc for maximum protection
set backup                    " keep backup files
set backupdir=~/.vim/backups  " store backups in dedicated dir
set undofile                  " persistent undo history
set undodir=~/.vim/undodir    " store undo files separately
set swapfile                  " ensure swap files are enabled (default)
set updatecount=100           " write to swap every 100 characters typed
set updatetime=4000           " write to swap every 4 seconds of inactivity
```

**Explanation:** The combination of swap files + backup files + persistent undo gives the best protection. But if I had to choose just one mechanism, swap files are the winner because they require zero configuration and capture the most recent state of the file including unsaved edits.

### Comparison Table

| Mechanism | Captures Unsaved Changes? | Requires Config? | Survives Crash? | Reliability |
|---|---|---|---|---|
| Swap files (.swp) | Yes | No (default on) | Yes | **Highest** |
| Backup files (~) | No (last save only) | Yes | Yes | Medium |
| Persistent Undo | Partially | Yes | Yes | Medium |
| Registers | N/A | No | **No** | Very Low |
| Auto-recovery | Yes (uses swap) | No | Yes | **Highest** |

### Why Swap Files Win:
1. Enabled by default - works even if you never configured vi
2. Continuously updated during editing (not just on `:w`)
3. Stored on disk, so they survive process crashes and system reboots  
4. Vi has built-in recovery workflow (the `E325` prompt)
5. Can list all recoverable files system-wide with `vi -r`

The main risk with swap files is if the disk itself fails (since the swap file is on the same disk). For critical files, combining swap files with periodic `:w` saves and external backups (like version control with git) provides the most comprehensive protection.
