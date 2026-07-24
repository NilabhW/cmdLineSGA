" Sample vimrc with recovery settings
" This configuration provides maximum protection against data loss

set nocompatible          " use vim defaults, not vi

" --- Swap File Settings ---
set swapfile              " enable swap files (default, but being explicit)
set updatecount=100       " write to swap every 100 chars typed
set updatetime=4000       " write to swap after 4 sec of inactivity

" --- Backup Settings ---
set backup                " keep a backup file
set backupdir=~/.vim/backups//  " store backups in dedicated directory

" --- Persistent Undo ---
set undofile              " save undo history to file
set undodir=~/.vim/undodir//    " store undo files separately
set undolevels=1000       " max number of undo levels

" Make sure backup and undo directories exist
silent !mkdir -p ~/.vim/backups ~/.vim/undodir
