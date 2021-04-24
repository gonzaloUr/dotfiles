let link = 'https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim'
let data_dir = stdpath('data') . '/site'

if empty(glob(data_dir . '/autoload/plug.vim'))
  silent execute '!curl -fLo ' . data_dir . '/autoload/plug.vim --create-dirs ' . link
endif

call plug#begin(stdpath('data') . '/plugged')
Plug 'junegunn/fzf'
Plug 'chrisbra/colorizer'
Plug 'fatih/vim-go', { 'do': ':GoUpdateBinaries' }
call plug#end()

filetype indent plugin on
syntax on
set hlsearch
set ignorecase
set smartcase
set number relativenumber
set tabstop=4
set shiftwidth=4
set expandtab

set undofile
set undodir=/tmp

" remove trailing whitespaces
au BufWritePre * %s/\s\+$//e

" remove | characters from the vertical split separator
set fillchars=

" better preview window
set splitbelow
au InsertLeave,CompleteDone * if pumvisible() == 0 | pclose | endif

" golang
au FileType go set expandtab&

" runs after the default syntax file for tex
"
" syntax files are vim scripts that run every time a buffer filetype is set and
" they setup all the necessary options for highlighting, which not only includes
" determining keywords and the like but entire syntactical entities for the
" given filetype (for example: functions and classes in programming languages)
augroup ft_tex
  " calls some tex.vim functions that make amsmath environments be treated as
  " traditional tex math syntax.
  autocmd Syntax tex call TexNewMathZone("E", "align", 1)
  autocmd Syntax tex call TexNewMathZone("F", "multline", 1)
  autocmd Syntax tex call TexNewMathZone("H", "equation", 1)
  autocmd Syntax tex call TexNewMathZone("I", "gather", 1)

  " tex compared to other formats is very complicated, detecting syntactical
  " entities requires parsing some parts of the file (for example lines close to
  " the current position of the cursor) or searching for keywords (for example C
  " like comments in programming languages), detecting theses entities is
  " essential for knowing where to spell check, for example in a programming
  " language you wouldn't treat var or int as a typo, because of how complicated
  " tex is if a file is too big tex.vim can fail at this task resulting in parts
  " of the file being skipped from spellchecking, something pretty essential
  " for writing tex documents, the following options make it so that there is
  " less trouble with that.
  "
  " See :help :syn-sync

  " spellcheck the toplevel syntactical entity
  autocmd Syntax tex syntax spell toplevel

  " parsing always starts at least this many lines backwards
  autocmd Syntax tex syntax sync minlines=50

  " parsing always stops after this many lines backwards from minlines
  autocmd Syntax tex syntax sync maxlines=500

  " these values where taken from
  " https://github.com/lervag/vimtex/blob/master/autoload/vimtex/syntax/core.vim
augroup end

" bindings
let mapleader="\<Space>"
let maplocalleader="-"

nnoremap <leader>9 :w \| bp<CR>
nnoremap <leader>0 :w \| bn<CR>
nnoremap <leader><localleader> :noh<CR>
nnoremap <leader><leader> :FZF<CR>

nnoremap <leader>s :set nospell<CR>
nnoremap <leader>e :set spell spelllang=es<CR>
nnoremap <leader>i :set spell spelllang=en<CR>

function PreviewPdf()
  ! xdg-open %:r.pdf >/dev/null 2>&1 & disown
endfunction

function Latexmk()
  ! st -e latexmk -pdf -pvc -view=none -interaction=nonstopmode "%" & disown
endfunction

nnoremap <leader>p :call PreviewPdf()<CR>
nnoremap <leader>l :call Latexmk()<CR>
nnoremap <leader>c :call Latexmk()<CR> :call PreviewPdf()<CR>
