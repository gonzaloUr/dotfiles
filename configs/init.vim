let link = 'https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim'
let data_dir = stdpath('data') . '/site'
let plug_path = data_dir . '/autoload/plug.vim'

if empty(glob(plug_path))
    execute '!curl -fLo ' . plug_path . ' --create-dirs ' . link
endif

call plug#begin(data_dir)
" neovim official extensions
Plug 'neovim/nvim-lspconfig'

" general extensions
Plug 'windwp/nvim-autopairs'
Plug 'folke/which-key.nvim'

" snippets support
Plug 'L3MON4D3/LuaSnip'
Plug 'rafamadriz/friendly-snippets'

" alternative to omnicomplete
Plug 'hrsh7th/nvim-cmp'
Plug 'hrsh7th/cmp-nvim-lsp'
Plug 'hrsh7th/cmp-buffer'
Plug 'hrsh7th/cmp-path'
Plug 'hrsh7th/cmp-cmdline'
Plug 'saadparwaiz1/cmp_luasnip'
call plug#end()

set completeopt=menu,menuone,noselect

lua <<EOF

  require'luasnip.loaders.from_vscode'.load()

  local cmp = require'cmp'

  cmp.setup({
    snippet = {
      expand = function(args)
        require('luasnip').lsp_expand(args.body)
      end,
    },
    mapping = {
      ['<tab>'] = cmp.mapping.select_next_item(),
      ['<S-tab>'] = cmp.mapping.select_prev_item(),
      ['<CR>']  = cmp.mapping.confirm({ select = true }),
    },
    sources = cmp.config.sources({
      { name = 'nvim_lsp' },
      { name = 'luasnip' },
    }, {
      { name = 'buffer' },
    })
  })

  -- Use buffer source for `/` (if you enabled `native_menu`, this won't work anymore).
  cmp.setup.cmdline('/', {
    sources = {
      { name = 'buffer' }
    }
  })

  -- Use cmdline & path source for ':' (if you enabled `native_menu`, this won't work anymore).
  cmp.setup.cmdline(':', {
    sources = cmp.config.sources({
      { name = 'path' }
    }, {
      { name = 'cmdline' }
    })
  })

  -- Setup lspconfig.
  local capabilities = require('cmp_nvim_lsp').update_capabilities(vim.lsp.protocol.make_client_capabilities())

  -- Replace <YOUR_LSP_SERVER> with each lsp server you've enabled.
  require('lspconfig')['gopls'].setup { capabilities = capabilities }
  require('lspconfig')['texlab'].setup { capabilities = capabilities }
  require('lspconfig')['tsserver'].setup { capabilities = capabilities }

  require('nvim-autopairs').setup{}
  local cmp_autopairs = require'nvim-autopairs.completion.cmp'

  cmp.event:on('confirm_done', cmp_autopairs.on_confirm_done({
    map_char = { tex = '' }
  }))

  cmp_autopairs.lisp[#cmp_autopairs.lisp+1] = "racket"
  cmp_autopairs.lisp[#cmp_autopairs.lisp+1] = "clojure"
  cmp_autopairs.lisp[#cmp_autopairs.lisp+1] = "clojurescript"
  cmp_autopairs.lisp[#cmp_autopairs.lisp+1] = "fennel"
  cmp_autopairs.lisp[#cmp_autopairs.lisp+1] = "janet"
EOF

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

let g:dracula_italic = 0
hi Normal guibg=NONE ctermbg=NONE

set tabstop=4
set shiftwidth=4
set expandtab

set laststatus=2
set statusline=%f\ %m
set statusline+=%=
set statusline+=%y\ %c
set statusline+=\ (%l
set statusline+=/
set statusline+=%L)

augroup indent_tabs
    autocmd FileType go set expandtab&
augroup end

augroup two_spaces
    autocmd FileType yaml set shiftwidth=2
    autocmd FileType lua set shiftwidth=2
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

    " parsing always stops after this many lines backwards from minlines
    autocmd Syntax tex syntax sync maxlines=500

    au FileType tex lua vim.api.nvim_buf_set_option(0, 'omnifunc', 'v:lua.vim.lsp.omnifunc')
augroup end

au BufWritePre * %s/\s\+$//e

set splitbelow
au InsertLeave,CompleteDone * if pumvisible() == 0 | pclose | endif

let mapleader="-"
let maplocalleader="\\"

nnoremap <leader>9 :w \| bp<CR>
nnoremap <leader>0 :w \| bn<CR>
nnoremap <leader><leader> :noh<CR>

nnoremap <leader>s :set nospell<CR>
nnoremap <leader>e :set spell spelllang=es<CR>
nnoremap <leader>i :set spell spelllang=en<CR>

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

set timeoutlen=200

lua <<EOF
  local wk = require('which-key')
  wk.register({
    ["<leader>"] = {
      e = {"<cmd>set spell spelllang=es", "Spellcheck español"},
      i = {"<cmd>set spell spelllang=en", "Spellcheck english"},
      s = {"<cmd>set nospell",            "Disable spellcheck"},
    }
  })

  wk.setup()
EOF

function MyTabLine()
    let s = ''
    for i in range(tabpagenr('$'))
        " select the highlighting
        if i + 1 == tabpagenr()
            let s .= '%#TabLineSel#'
        else
            let s .= '%#TabLine#'
        endif

        " set the tab page number (for mouse clicks)
        let s .= '%' . (i + 1) . 'T'

        " the label is made by MyTabLabel()
        let s .= ' %{MyTabLabel(' . (i + 1) . ')} '
    endfor

    " after the last tab fill with TabLineFill and reset tab page nr
    let s .= '%#TabLineFill#%T'

    return s
endfunction

function MyTabLabel(n)
    " windows to buffers
    let buflist = tabpagebuflist(a:n)

    " current window number
    let winnr = tabpagewinnr(a:n)

    " buffer name of the current window
    let buffname = bufname(buflist[winnr - 1])

    " if buffname is empty return [No Name]
    if buffname == ""
        return '[No Name]'
    else
        return buffname
endfunction

set tabline=%!MyTabLine()

lua <<EOF
vim.diagnostic.config({
  virtual_text = false
})

-- Show line diagnostics automatically in hover window
vim.o.updatetime = 250
vim.cmd [[autocmd CursorHold,CursorHoldI * lua vim.diagnostic.open_float(nil, {focus=false})]]
EOF
