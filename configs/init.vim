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
set splitbelow

augroup indent_tabs
  autocmd FileType go set expandtab&
augroup end

augroup two_spaces
  autocmd FileType yaml set shiftwidth=2
  autocmd FileType json set shiftwidth=2
  autocmd FileType lua set shiftwidth=2
  autocmd FileType dart set shiftwidth=2
  autocmd FileType vim set shiftwidth=2
  autocmd FileType html set shiftwidth=2
  autocmd FileType scss set shiftwidth=2
  autocmd FileType css set shiftwidth=2
  autocmd FileType typescript set shiftwidth=2
  autocmd FileType typescript set shiftwidth=2
  autocmd FileType typescriptreact set shiftwidth=2
  autocmd FileType javascript set shiftwidth=2
augroup end

augroup ft_md
  au BufNewFile,BufFilePre,BufRead *.md syn clear markdownItalic
augroup end

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

  " parsing always stops after this many lines forward from minlines
  autocmd Syntax tex syntax sync maxlines=500
augroup end

let mapleader=","
let maplocalleader="-"

nnoremap <leader><leader> :noh<CR>
nnoremap <leader>ls :set nospell<CR>
nnoremap <leader>le :set spell spelllang=es<CR>
nnoremap <leader>li :set spell spelllang=en<CR>

nnoremap <leader>gD :lua vim.lsp.buf.declaration()<CR>
nnoremap <leader>gd :lua vim.lsp.buf.definition()<CR>
nnoremap <leader>gi :lua vim.lsp.buf.implementation()<CR>
nnoremap <leader>a  :lua vim.lsp.buf.code_action()<CR>
nnoremap <leader>h  :lua vim.lsp.buf.hover()<CR>
nnoremap <leader>s  :lua vim.lsp.buf.signature_help()<CR>
nnoremap <leader>wa :lua vim.lsp.buf.add_workspace_folder()<CR>
nnoremap <leader>wr :lua vim.lsp.buf.remove_workspace_folder()<CR>
nnoremap <leader>wl :lua print(vim.inspect(vim.lsp.buf.list_workspace_folders()))<CR>
nnoremap <leader>re :lua vim.lsp.buf.rename()<CR>
nnoremap <leader>rf :lua vim.lsp.buf.references()<CR>
nnoremap <leader>f  :lua vim.lsp.buf.formatting()<CR>
nnoremap <leader>n  :lua vim.lsp.diagnostic.goto_next()<CR>
nnoremap <leader>p  :lua vim.lsp.diagnostic.goto_prev()<CR>
nnoremap <leader>e  :lua vim.diagnostic.open_float()<CR>

au BufWritePre * %s/\s\+$//e
au CompleteDone * if pumvisible() == 0 | pclose | endif

au Filetype go set omnifunc=v:lua.vim.lsp.omnifunc
au Filetype tex set omnifunc=v:lua.vim.lsp.omnifunc
au Filetype typescript set omnifunc=v:lua.vim.lsp.omnifunc

lua<<EOF
  local lspconfig = require('lspconfig')

  lspconfig.util.default_config = vim.tbl_extend("force", lspconfig.util.default_config, {
    handlers = {
      ['textDocument/publishDiagnostics'] = vim.lsp.with(
        vim.lsp.diagnostic.on_publish_diagnostics,
        {
          virtual_text = false,
          signs = true,
          update_in_insert = false,
          underline = true,
          float = {
            source = 'always',
          },
        }
      )
    }
  })

  lspconfig['gopls'].setup {}
  lspconfig['texlab'].setup {}
  lspconfig['tsserver'].setup {}
  lspconfig['angularls'].setup {}
  lspconfig['jdtls'].setup {}
EOF
