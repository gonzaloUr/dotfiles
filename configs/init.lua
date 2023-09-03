local lspconfig = require 'lspconfig'

local indentTabs = vim.api.nvim_create_augroup('IndentTabs')
vim.api.nvim_create_autocmd('FileType', { pattern = 'go', group = indentTabs, command = 'set expandtab&' })

local twoSpaces = vim.api.nvim_create_augroup('TwoSpaces')
vim.api.nvim_create_autocmd('FileType', { pattern = 'yaml', group = twoSpaces, command = 'set shiftwidth=2' })
vim.api.nvim_create_autocmd('FileType', { pattern = 'json', group = twoSpaces, command = 'set shiftwidth=2' })
vim.api.nvim_create_autocmd('FileType', { pattern = 'lua', group = twoSpaces, command = 'set shiftwidth=2' })
vim.api.nvim_create_autocmd('FileType', { pattern = 'dart', group = twoSpaces, command = 'set shiftwidth=2' })
vim.api.nvim_create_autocmd('FileType', { pattern = 'vim', group = twoSpaces, command = 'set shiftwidth=2' })
vim.api.nvim_create_autocmd('FileType', { pattern = 'html', group = twoSpaces, command = 'set shiftwidth=2' })
vim.api.nvim_create_autocmd('FileType', { pattern = 'scss', group = twoSpaces, command = 'set shiftwidth=2' })
vim.api.nvim_create_autocmd('FileType', { pattern = 'css', group = twoSpaces, command = 'set shiftwidth=2' })
vim.api.nvim_create_autocmd('FileType', { pattern = 'typescript', group = twoSpaces, command = 'set shiftwidth=2' })
vim.api.nvim_create_autocmd('FileType', { pattern = 'typescript', group = twoSpaces, command = 'set shiftwidth=2' })
vim.api.nvim_create_autocmd('FileType', { pattern = 'typescriptreact', group = twoSpaces, command = 'set shiftwidth=2' })
vim.api.nvim_create_autocmd('FileType', { pattern = 'javascript', group = twoSpaces, command = 'set shiftwidth=2' })

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

lspconfig['gopls'].setup {}
lspconfig['texlab'].setup {}
lspconfig['tsserver'].setup {}
lspconfig['angularls'].setup {}
lspconfig['jdtls'].setup {}
