let link = 'https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim'
let data_dir = stdpath('data') . '/site'
let plug_path = data_dir . '/autoload/plug.vim'

if empty(glob(plug_path))
    execute '!curl -fLo ' . plug_path . ' --create-dirs ' . link
endif

call plug#begin(data_dir)
Plug 'https://github.com/jiangmiao/auto-pairs'

Plug 'chrisbra/colorizer'
Plug 'scrooloose/nerdtree'
Plug 'junegunn/fzf.vim'
Plug 'sebdah/vim-delve'

Plug 'neovim/nvim-lspconfig'
Plug 'simrat39/symbols-outline.nvim'

Plug 'L3MON4D3/LuaSnip'
Plug 'rafamadriz/friendly-snippets'
Plug 'hrsh7th/nvim-cmp'
Plug 'hrsh7th/cmp-nvim-lsp'
Plug 'saadparwaiz1/cmp_luasnip'
call plug#end()

set completeopt=menu,menuone,noselect
lua <<EOF
    local cmp = require'cmp'

    cmp.setup({
        snippet = {
            expand = function(args)
                require('luasnip').lsp_expand(args.body)
            end,
        },
        mapping = {
            ['<C-n>'] = cmp.mapping.select_next_item(),
            ['<C-p>'] = cmp.mapping.select_prev_item(),
            ['<CR>']  = cmp.mapping.confirm({ select = true }),
        },
        sources = cmp.config.sources({
            { name = 'nvim_lsp' },
            { name = 'luasnip' },
        }, {}),
    })

    local capabilities = require('cmp_nvim_lsp').update_capabilities(vim.lsp.protocol.make_client_capabilities())
    require('lspconfig')['gopls'].setup { capabilities = capabilities }
    require('lspconfig')['texlab'].setup { capabilities = capabilities }

    require'luasnip'.filetype_extend("go", {"go"})
    require'luasnip'.filetype_extend("latex", {"latex-snippets"})
    require'luasnip.loaders.from_vscode'.load()
EOF

" augroup ft_python
"     au FileType python lua vim.api.nvim_buf_set_option(0, 'omnifunc', 'v:lua.vim.lsp.omnifunc')
" augroup end
"
" augroup ft_go
"     au FileType go lua vim.api.nvim_buf_set_option(0, 'omnifunc', 'v:lua.vim.lsp.omnifunc')
" augroup end
"
" augroup ft_tex
"     au FileType tex lua vim.api.nvim_buf_set_option(0, 'omnifunc', 'v:lua.vim.lsp.omnifunc')
" augroup end

filetype indent plugin on
syntax on
set hlsearch
set title
set ignorecase
set smartcase
set number relativenumber
set undofile
set undodir=/tmp
set mouse=a
set clipboard+=unnamedplus

set tabstop=4
set shiftwidth=4
set expandtab

augroup ft_go
    autocmd FileType go set expandtab&
augroup end

augroup ft_yaml
    autocmd FileType yaml set shiftwidth=2
augroup end

set laststatus=2
set statusline=%f\ %m
set statusline+=%=
set statusline+=%y\ %c
set statusline+=\ (%l
set statusline+=/
set statusline+=%L)

" https://github.com/neovim/neovim/issues/11330
autocmd VimEnter * silent exec "!kill -s SIGWINCH" getpid()

" remove trailing whitespaces
au BufWritePre * %s/\s\+$//e

" better preview window
set splitbelow
au InsertLeave,CompleteDone * if pumvisible() == 0 | pclose | endif

" vim tex syntax file settings
augroup ft_tex
    " Make amsmath environments traditional tex math enviroments
    autocmd Syntax tex call TexNewMathZone("E", "align", 1)
    autocmd Syntax tex call TexNewMathZone("F", "multline", 1)
    autocmd Syntax tex call TexNewMathZone("H", "equation", 1)
    autocmd Syntax tex call TexNewMathZone("I", "gather", 1)

    " spellcheck the toplevel syntactical entity (outside begin document)
    autocmd Syntax tex syntax spell toplevel

    " parsing always starts at least this many lines backwards
    autocmd Syntax tex syntax sync minlines=50

    " parsing always stops after this many lines backwards from minlines
    autocmd Syntax tex syntax sync maxlines=500
augroup end

" functions
function ToggleColorColumn()
    if &colorcolumn == ""
        set colorcolumn=80
    else
        set colorcolumn&
    endif
endfunction

function PreviewPdf()
    ! xdg-open %:r.pdf >/dev/null 2>&1 & disown
endfunction

function Dvisvgm()
    ! [ -e %:r-1.svg ] && rm %:r-1.svg
    ! dvisvgm --font-format=woff2 %:r.dvi >/dev/null 2>&1 & disown
endfunction

function Latexmk()
    ! $TERMINAL -e latexmk -pdf -pvc -view=none -interaction=nonstopmode "%" & disown
endfunction

function LatexmkDvi()
    let f = expand('%:t')
    tabnew latexmk
    exec 'terminal latexmk -dvi -pvc -view=none -interaction=nonstopmode '.f
endfunction

function PandocMarkdown()
    let f = expand('%:t')
    let out = expand('%:t:r')
    tabnew
    exec 'terminal echo ' . f . ' | entr pandoc --mathjax -s -i ' . f . ' -o ' . out . '.html'
    exec 'file pandoc ' . f
endfunction

" bindings
let mapleader="-"
let maplocalleader="\\"

nnoremap <leader>r :call ToggleColorColumn()<CR>
nnoremap <leader>9 :w \| bp<CR>
nnoremap <leader>0 :w \| bn<CR>
nnoremap <leader><leader> :noh<CR>

nnoremap <leader>s :set nospell<CR>
nnoremap <leader>e :set spell spelllang=es<CR>
nnoremap <leader>i :set spell spelllang=en<CR>

augroup ft_tex
    au Filetype tex nnoremap <leader>p :call PreviewPdf()<CR>
    au Filetype tex nnoremap <leader>l :call Latexmk()<CR>
    au Filetype tex nnoremap <leader>L :call LatexmkDvi()<CR>
    au Filetype tex nnoremap <leader>c :call Latexmk()<CR> :call PreviewPdf()<CR>
    au Filetype tex nnoremap <leader>C :call Dvisvgm()<CR>
augroup end

nnoremap <leader>P :call PandocMarkdown()<CR>

nnoremap <C-n> :NERDTreeToggle<CR>
nnoremap <C-g> :NERDTreeFind<CR>

nnoremap <leader><localleader> :Files<CR>
nnoremap <leader>g :GFiles<CR>
nnoremap <leader>b :Buffers<CR>
nnoremap <leader>t :Filetypes<CR>
nnoremap <leader>m :Maps<CR>
nnoremap <leader>M :Marks<CR>
nnoremap <leader>C :Colors<CR>
nnoremap <leader>L :Lines<CR>
nnoremap <leader>l :BLines<CR>

nnoremap ,gD :lua vim.lsp.buf.declaration()<CR>
nnoremap ,gd :lua vim.lsp.buf.definition()<CR>
nnoremap ,gi :lua vim.lsp.buf.implementation()<CR>
nnoremap ,a  :lua vim.lsp.buf.code_action()<CR>
nnoremap ,h  :lua vim.lsp.buf.hover()<CR>
nnoremap ,sh :lua vim.lsp.buf.signature_help()<CR>
nnoremap ,sd :lua vim.lsp.diagnostic.show_line_diagnostics()<CR>
nnoremap ,wa :lua vim.lsp.buf.add_workspace_folder()<CR>
nnoremap ,wr :lua vim.lsp.buf.remove_workspace_folder()<CR>
nnoremap ,wl :lua print(vim.inspect(vim.lsp.buf.list_workspace_folders()))<CR>
nnoremap ,td :lua vim.lsp.buf.type_definition()<CR>
nnoremap ,re  :lua vim.lsp.buf.rename()<CR>
nnoremap ,rf :lua vim.lsp.buf.references()<CR>
nnoremap ,f  :lua vim.lsp.buf.formatting()<CR>
nnoremap ,n  :lua vim.lsp.diagnostic.goto_next()<CR>
nnoremap ,p  :lua vim.lsp.diagnostic.goto_prev()<CR>
