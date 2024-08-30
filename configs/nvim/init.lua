require('packer').startup(function(use)
  -- oficial plugins.
  use 'neovim/nvim-lspconfig'

  -- third party lua plugins.
  use 'folke/which-key.nvim'
  use 'nvim-tree/nvim-tree.lua'
  use 'lewis6991/gitsigns.nvim'
  use 'Mofiqul/vscode.nvim'
  use { 
    'hrsh7th/nvim-cmp',
    requires = {
      -- Completation sources, see nvim-cmp setup sources.
      'hrsh7th/cmp-nvim-lsp',
      'hrsh7th/cmp-path',
      'hrsh7th/cmp-buffer'
    }
  }
end)

-- options and global variables.

vim.opt.wrap = false
vim.opt.number = true
vim.opt.relativenumber = true
vim.opt.tabstop = 4
vim.opt.shiftwidth = 4
vim.opt.splitbelow = true
vim.opt.expandtab = true
vim.opt.ignorecase = true

-- vscode.nvim

local vscode = require('vscode')

vscode.setup({
  transparent = true,
  italic_comments = true
})

vscode.load()

-- nvim-tree plugin.

vim.g.loaded_netrw = 1
vim.g.loaded_netrwPlugin = 1

require('nvim-tree').setup()

-- gitsigns.nvim.

require('gitsigns').setup()

-- lsp.

local lspconfig = require('lspconfig')
local configs = require('lspconfig.configs')
local util = require('lspconfig.util')

util.default_config = vim.tbl_extend('force', util.default_config, {
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
lspconfig['pyright'].setup {}
lspconfig['texlab'].setup {}

-- nvim-cmp.

local cmp = require('cmp')

cmp.setup {
  mapping = {
    -- https://github.com/neovim/nvim-lspconfig/wiki/Autocompletion#nvim-cmp
    ['<C-u>'] = cmp.mapping.scroll_docs(-4),
    ['<C-d>'] = cmp.mapping.scroll_docs(4),
    ['<C-Space>'] = cmp.mapping.complete(),
    ['<CR>'] = cmp.mapping.confirm {
      behavior = cmp.ConfirmBehavior.Replace,
      select = true,
    },
    ['<Tab>'] = cmp.mapping(function(fallback)
      if cmp.visible() then
        cmp.select_next_item()
      else
        fallback()
      end
    end, { 'i', 's' }),
    ['<S-Tab>'] = cmp.mapping(function(fallback)
      if cmp.visible() then
        cmp.select_prev_item()
      else
        fallback()
      end
    end, { 'i', 's' })
  },
  sources = {
    { name = 'nvim_lsp' },
    { name = 'path' },
    { name = 'buffer' }
  }
}

-- mappings and which key.

vim.g.mapleader = ','
vim.g.maplocalleader = '-'

vim.keymap.set('n', '<Leader><Leader>', '<cmd>noh<cr>')
vim.keymap.set('n', '<Leader>.', '<cmd>NvimTreeToggle<cr>')

local wk = require('which-key')

wk.register({
  l = {
    name = 'Lsp',
    D = {function() vim.lsp.buf.declaration() end, 'Go to declaration'},
    d = {function() vim.lsp.buf.definition() end, 'Go to definition'},
    i = {function() vim.lsp.buf.implementation() end, 'Go to implementation'},
    a = {function() vim.lsp.buf.code_action() end, 'Code Action'},
    h = {function() vim.lsp.buf.hover() end, 'Hover'},
    s = {function() vim.lsp.buf.signature_help() end, 'Show signature'},
    af = {function() vim.lsp.buf.add_workspace_folder() end, 'Add file path to lsp root'},
    rf = {function() vim.lsp.buf.remove_workspace_folder() end, 'Remove file path from lsp root'},
    lf = {function() print(vim.inspect(vim.lsp.buf.list_workspace_folders())) end, 'List dirs as root in lsp'},
    r = {function() vim.lsp.buf.rename() end, 'Rename'},
    u = {function() vim.lsp.buf.references() end, 'Usages'},
    f = {function() vim.lsp.buf.format() end, 'Format'},
    n = {function() vim.diagnostic.goto_next() end, 'Go next error'},
    p = {function() vim.diagnostic.goto_prev() end, 'Go prev error'},
    E = {function() vim.diagnostic.open_float() end, 'Show errors'},
  },
}, { prefix = '<Leader>' })

wk.setup()

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
    'javascript'
  },
  callback = function(args)
    vim.bo[args.buf].shiftwidth = 2
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
