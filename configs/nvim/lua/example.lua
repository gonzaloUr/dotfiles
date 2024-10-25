local M = {}

function M.test_completation()
  local line = vim.api.nvim_get_current_line()
  local cursor_pos = vim.api.nvim_win_get_cursor(0)[2]
  local completation_items = {'test1', 'test2'}
  vim.fn.complete(cursor_pos, completation_items)
end

-- require('example').setup() from init.lua
function M.setup()
  vim.keymap.set('i', '<C-c>', M.test_completation)
end

return M
