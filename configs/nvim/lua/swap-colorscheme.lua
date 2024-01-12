local M = {
  current = nil,
}

function M.get_current_color_index(colors)
  for i = 0, #colors do
    if colors[i] == vim.g.colors_name then
      current = i
      break
    end
  end

  return current
end

function M.set_current()
  if M.current ~= nil then
    return
  end

  colors = vim.fn.getcompletion('', 'color')
  M.current = M.get_current_color_index(colors)
end

function M.next_color()
  M.set_current()

  if current == #colors then
    current = 1
  else
    current = current + 1
  end

  vim.cmd.colorscheme(colors[current])
  print(colors[current] .. ' ' .. current .. '/' .. #colors)
end

function M.prev_color()
  M.set_current()

  if current == 1 then
    current = #colors
  else
    current = current - 1
  end

  vim.cmd.colorscheme(colors[current])
  print(colors[current] .. ' ' .. current .. '/' .. #colors)
end

function M.setup()
  M.set_current()
  vim.keymap.set('n', '>', M.next_color)
  vim.keymap.set('n', '<', M.prev_color)
end

return M
