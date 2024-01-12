require ('packer').startup(function(use)
  -- oficial plugins.
  use 'neovim/nvim-lspconfig'

  -- third party vimscript plugins.
  use 'rafi/awesome-vim-colorschemes'
  use 'airblade/vim-gitgutter'

  -- third party lua plugins.
  use 'folke/which-key.nvim'
  use 'tomtomjhj/vscoq.nvim'
end)

require('which-key').setup()
require('swap-colorscheme').setup()
vim.cmd.colorscheme('purify')

-- options and global variables.

vim.opt.number = true
vim.opt.relativenumber = true
vim.opt.tabstop = 4
vim.opt.shiftwidth = 4
vim.opt.splitbelow = true
vim.opt.expandtab = true
vim.opt.ignorecase = true

-- lsp.

local lspconfig = require('lspconfig')

lspconfig.util.default_config = vim.tbl_extend('force', lspconfig.util.default_config, {
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

lspconfig['gopls'].setup {
  settings = {
    gopls = {
      staticcheck = true
    }
  }
}
lspconfig['texlab'].setup {}
lspconfig['tsserver'].setup {}
lspconfig['angularls'].setup {}
lspconfig['jdtls'].setup {}

require('vscoq').setup {}

-- mappings

vim.g.mapleader = ','
vim.g.maplocalleader = '-'

vim.keymap.set('n', '<Leader><Leader>', '<cmd>noh<cr>')
vim.keymap.set('n', '<Leader>gD', function() vim.lsp.buf.declaration() end)
vim.keymap.set('n', '<Leader>gd', function() vim.lsp.buf.definition() end)
vim.keymap.set('n', '<Leader>gi', function() vim.lsp.buf.implementation() end)
vim.keymap.set('n', '<Leader>a', function() vim.lsp.buf.code_action() end)
vim.keymap.set('n', '<Leader>h', function() vim.lsp.buf.hover() end)
vim.keymap.set('n', '<Leader>s', function() vim.lsp.buf.signature_help() end)
vim.keymap.set('n', '<Leader>wa', function() vim.lsp.buf.add_workspace_folder() end)
vim.keymap.set('n', '<Leader>wr', function() vim.lsp.buf.remove_workspace_folder() end)
vim.keymap.set('n', '<Leader>wl', function() print(vim.inspect(vim.lsp.buf.list_workspace_folders())) end)
vim.keymap.set('n', '<Leader>re', function() vim.lsp.buf.rename() end)
vim.keymap.set('n', '<Leader>rf', function() vim.lsp.buf.references() end)
vim.keymap.set('n', '<Leader>f', function() vim.lsp.buf.formatting() end)
vim.keymap.set('n', '<Leader>l', function() vim.diagnostic.setqflist() end)
vim.keymap.set('n', '<Leader>n', function() vim.diagnostic.goto_next() end)
vim.keymap.set('n', '<Leader>p', function() vim.diagnostic.goto_prev() end)
vim.keymap.set('n', '<Leader>e', function() vim.diagnostic.open_float() end)

-- autocmds

vim.api.nvim_create_autocmd('BufWritePre', {
  pattern = '*',
  callback = function()
    vim.cmd.substitute('/\\s\\+$//e')
  end
})

vim.api.nvim_create_autocmd('CompleteDone', {
  callback = function()
    if vim.fn.pumvisible() then
      vim.api.nvim_command("pclose")
    end
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
    'coq'
  },
  callback = function(args)
    vim.bo[args.buf].shiftwidth = 2
  end
})

vim.api.nvim_create_autocmd('FileType', {
  pattern = 'coq',
  callback = function(args)
    vim.bo[args.buf].keymap = 'unicode-math'
  end
})
