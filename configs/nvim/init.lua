require('packer').startup(function(use)
  -- oficial plugins.
  use 'neovim/nvim-lspconfig'

  -- third party lua plugins.
  use 'folke/which-key.nvim'
  use 'nvim-tree/nvim-tree.lua'
  use 'catppuccin/nvim'
  use 'mfussenegger/nvim-dap'
  use 'leoluz/nvim-dap-go'

  -- vimscript plugins.
  use 'whonore/Coqtail'
  use 'maelvalais/gmpl.vim'
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

-- colorscheme

require('catppuccin').setup({
  transparent_background = true
})

vim.cmd.colorscheme('catppuccin-macchiato')

-- nvim-tree plugin.

require('nvim-tree').setup({
  sort_by = 'extension'
})

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
lspconfig['r_language_server'].setup {}

-- nvim-dap y nvim-dap-go.
local dap = require('dap')

require('dap-go').setup {
  dap_configurations = {
    {
      type = "go",
      name = "Attach remote",
      mode = "remote",
      request = "attach"
    }
  },
  delve = {
    path = "dlv",
    initialize_timeout_sec = 20,
    port = "${port}",
    args = {},
    build_flags = {},
    cwd = nil
  },
  tests = {
    verbose = false
  }
}

-- mappings and which key.

vim.g.mapleader = ','
vim.g.maplocalleader = '-'

vim.keymap.set('n', '<Leader><Leader>', '<cmd>noh<cr>')
vim.keymap.set('n', '<Leader>.', '<cmd>NvimTreeToggle<cr>')

local wk = require('which-key')

wk.add({
    -- lsp keybindings.
    { '<Leader>l', group = 'Lsp' },
    { '<Leader>lD', function() vim.lsp.buf.declaration() end, desc = 'Go to declaration' },
    { '<Leader>ld', function() vim.lsp.buf.definition() end, desc = 'Go to definition' },
    { '<Leader>li', function() vim.lsp.buf.implementation() end, desc = 'Go to implementation' },
    { '<Leader>la', function() vim.lsp.buf.code_action() end, desc = 'Code Action' },
    { '<Leader>lh', function() vim.lsp.buf.hover() end, desc = 'Hover' },
    { '<Leader>ls', function() vim.lsp.buf.signature_help() end, desc = 'Show signature' },
    { '<Leader>laf', function() vim.lsp.buf.add_workspace_folder() end, desc = 'Add file path to lsp root' },
    { '<Leader>lrf', function() vim.lsp.buf.remove_workspace_folder() end, desc = 'Remove file path from lsp root' },
    { '<Leader>llf', function() print(vim.inspect(vim.lsp.buf.list_workspace_folders())) end, desc = 'List dirs as root in lsp' },
    { '<Leader>lr', function() vim.lsp.buf.rename() end, desc = 'Rename' },
    { '<Leader>lu', function() vim.lsp.buf.references() end, desc = 'Usages' },
    { '<Leader>lf', function() vim.lsp.buf.format() end, desc = 'Format' },
    { '<Leader>ln', function() vim.diagnostic.goto_next() end, desc = 'Go next error' },
    { '<Leader>lp', function() vim.diagnostic.goto_prev() end, desc = 'Go prev error' },
    { '<Leader>lE', function() vim.diagnostic.open_float() end, desc = 'Show errors' },

    -- nvim-dap keybindings.
    { '<Leader>d', group = 'Debugger' },
    { '<Leader>d<F5>', function() require('dap').continue() end, desc = 'Continue' },
    { '<Leader>d<F10>', function() require('dap').step_over() end, desc = 'Step over' },
    { '<Leader>d<F11>', function() require('dap').step_into() end, desc = 'Step into' },
    { '<Leader>d<F12>', function() require('dap').step_out() end, desc = 'Step out' },
    { '<Leader>db', function() require('dap').toggle_breakpoint() end, desc = 'Toggle breakpoint' },
    { '<Leader>dB', function() require('dap').set_breakpoint() end, desc = 'Set breakpoint' },
    { '<Leader>dr', function() require('dap').repl.open() end, desc = 'Open repl' },
    { '<Leader>dh', function() require('dap.ui.widgets').preview() end, desc = 'Hover in preview window' }
});

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
      vim.api.nvim_command('pclose')
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

vim.api.nvim_create_autocmd('FileType', {
  pattern = {
    'tex'
  },
  callback = function(args)
    vim.wo.wrap = true
  end
})

vim.api.nvim_create_autocmd('VimResized', {
  pattern = '*',
  callback = function(args)
    vim.api.nvim_command('wincmd =')
  end
})
