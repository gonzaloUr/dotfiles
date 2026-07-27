-- bootstrap lazyvim.
local lazypath = vim.fn.stdpath("data") .. "/lazy/lazy.nvim"

if not (vim.uv or vim.loop).fs_stat(lazypath) then
  local lazyrepo = "https://github.com/folke/lazy.nvim.git"
  local out = vim.fn.system({ "git", "clone", "--filter=blob:none", "--branch=stable", lazyrepo, lazypath })

  if vim.v.shell_error ~= 0 then
    vim.api.nvim_echo({ { "Failed to clone lazy.nvim:\n"}, { out } }, true, {})
    vim.fn.getchar()
    os.exit(1)
  end
end

vim.opt.rtp:prepend(lazypath)

-- its mandatory to setup mapleader and maplocalleader before seting up lazy
vim.g.mapleader = ','
vim.g.maplocalleader = '-'

-- setup lazy.nvim
require('lazy').setup {
  -- official neovim lua plugins
  { 'neovim/nvim-lspconfig', version = '*' },

  -- third party lua plugins
  { 'folke/which-key.nvim', version = '*' },
  { 'nvim-tree/nvim-tree.lua', version = '*' },
  { 'nvim-telescope/telescope.nvim', version = '*', dependencies = { 'nvim-lua/plenary.nvim' } },
  { 'sindrets/diffview.nvim', version = '*' },
  { 'lewis6991/gitsigns.nvim', version = '*' },
  { 'Zeta611/tex2uni.nvim', version = '*', opts = { ft = { '*.v' } } },
  { 'nvim-mini/mini.completion', version = '*' },

  -- third party vimscript plugins
  { 'whonore/Coqtail', version = '*' },

  -- development.
  -- { dir = '~/Dropbox/nvim-plugins/nvim-vsrocq', name = 'nvim-vsrocq' }
}

-- development.
-- vim.g.loaded_coqtail = 1
-- vim.g["coqtail#supported"] = 0
-- require('nvim-vsrocq').setup()
-- vim.lsp.enable('vsrocqtop')

-- colorscheme.
vim.api.nvim_set_hl(0, "Normal", { bg = "NONE" })

-- setup options
vim.opt.wrap = false
vim.opt.number = true
vim.opt.relativenumber = true
vim.opt.tabstop = 4
vim.opt.shiftwidth = 4
vim.opt.splitbelow = true
vim.opt.expandtab = true
vim.opt.ignorecase = true

-- setup nvim-tree
require('nvim-tree').setup {
  sort = {
    sorter = 'extension'
  }
}

-- setup telescope
require('telescope').setup {}

-- setup diffview.
require('diffview').setup {
  use_icons = false
}

-- setup mini.completion
require('mini.completion').setup {}

vim.keymap.set('i', '<Tab>', [[pumvisible() ? "\<C-n>" : "\<Tab>"]], { expr = true })
vim.keymap.set('i', '<S-Tab>', [[pumvisible() ? "\<C-p>" : "\<S-Tab>"]], { expr = true })

-- setup mappings and then which key
local builtin = require('telescope.builtin')

vim.keymap.set('n', '<Leader><Leader>', '<cmd>noh<CR>', { desc = 'Clear search history' })
vim.keymap.set('n', '<Leader>.', '<cmd>NvimTreeToggle<CR>', { desc = 'Toggle NvimTree' })
vim.keymap.set('n', '<Leader>a', vim.lsp.buf.code_action, { desc = 'Code Action' })
vim.keymap.set('n', '<Leader>d', vim.lsp.buf.definition, { desc = 'Go to definition' })
vim.keymap.set('n', '<Leader>D', vim.lsp.buf.declaration, { desc = 'Go to declaration' })
vim.keymap.set('n', '<Leader>i', vim.lsp.buf.implementation, { desc = 'Go to implementation' })
vim.keymap.set('n', '<Leader>e', vim.diagnostic.open_float, { desc = 'Show errors' })
vim.keymap.set('n', '<Leader>f', vim.lsp.buf.format, { desc = 'Format' })
vim.keymap.set('n', '<Leader>h', vim.lsp.buf.hover, { desc = 'Hover' })
vim.keymap.set('n', '<Leader>r', vim.lsp.buf.rename, { desc = 'Rename' })
vim.keymap.set('n', '<Leader>s', vim.lsp.buf.signature_help, { desc = 'Show signature' })
vim.keymap.set('n', '<Leader>u', vim.lsp.buf.references, { desc = 'Usages' })
vim.keymap.set('n', '<Leader>n', function() vim.diagnostic.jump({ count = 1 }) end, { desc = 'Go to next error' })
vim.keymap.set('n', '<Leader>p', function() vim.diagnostic.jump({ count = -1 }) end, { desc = 'Go to previous error' })
vim.keymap.set('n', '<Leader>t', builtin.find_files, { desc = 'Telescope find files' })
vim.keymap.set('n', '<Leader>b', builtin.buffers, { desc = 'Telescope buffers' })

require('which-key').setup {}

-- config and enable lsp configs some from nvim-lspconfig.
vim.lsp.config('lua_ls', {
  settings = {
    Lua = {
      diagnostics = {
        globals = { 'vim' }
      }
    }
  }
})

vim.lsp.config('ltex_plus', {
  settings = {
    ltex = {
      language = "es-AR",
      latex = {
        environments = {
          minted = "ignore",
          mathpar = "ignore"
        }
      }
    }
  }
})

vim.lsp.enable('ltex_plus')
vim.lsp.enable('gopls')
vim.lsp.enable('pyright')
vim.lsp.enable('texlab')
vim.lsp.enable('r_language_server')
vim.lsp.enable('ocamllsp')
vim.lsp.enable('hls')
vim.lsp.enable('elixirls')
vim.lsp.enable('lua_ls')

-- custom filetypes
vim.filetype.add({
  extension = {
    mlg = 'mlg'
  }
})

-- autocmds
vim.api.nvim_create_autocmd('BufWritePre', {
  pattern = '*',
  callback = function()
    vim.cmd([[%s/\s\+$//e]])
  end
})

vim.api.nvim_create_autocmd('CompleteDone', {
  callback = function()
    if vim.fn.pumvisible() then
      vim.api.nvim_command('pclose')
    end
  end
})

vim.api.nvim_create_autocmd('FileType', {
  pattern = {
    'go'
  },
  callback = function(args)
    vim.bo[args.buf].expandtab = false
    vim.bo[args.buf].tabstop = 4
    vim.bo[args.buf].shiftwidth = 4
  end
})

vim.api.nvim_create_autocmd('FileType', {
  pattern = 'coq',
  callback = function()
    vim.cmd('highlight Error NONE')
  end,
})

vim.api.nvim_create_autocmd('FileType', {
  pattern = 'mlg',
  callback = function()
    vim.cmd('syntax enable')
    vim.cmd('setlocal syntax=ocaml')
  end
})

vim.api.nvim_create_autocmd('FileType', {
  pattern = {
    'lua',
    'yaml',
    'json',
    'dart',
    'vim',
    'html',
    'scss',
    'css',
    'typescript',
    'typescriptreact',
    'javascript',
    'markdown',
    'ocaml',
    'haskell',
    'tex',
    'mlg'
  },
  callback = function(args)
    vim.bo[args.buf].shiftwidth = 2
  end
})

vim.api.nvim_create_autocmd('FileType', {
  pattern = {
    'tex',
    'lhaskell'
  },
  callback = function(_)
    vim.wo.wrap = true
  end
})

vim.api.nvim_create_autocmd('VimResized', {
  pattern = '*',
  callback = function(_)
    vim.api.nvim_command('wincmd =')
  end
})
