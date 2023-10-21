function get_current_color_index(colors)
  for i = 0, #colors do
    if colors[i] == vim.g.colors_name then
      current = i
      break
    end
  end

  return current
end

function next_color()
  colors = vim.fn.getcompletion('', 'color')
  current = get_current_color_index(colors)

  if current == #colors then
    current = 1
  else
    current = current + 1
  end

  vim.cmd.colorscheme(colors[current])
  print(colors[current] .. ' ' .. current .. '/' .. #colors)
end

function prev_color()
  colors = vim.fn.getcompletion('', 'color')
  current = get_current_color_index(colors)

  if current == 1 then
    current = #colors
  else
    current = current - 1
  end

  vim.cmd.colorscheme(colors[current])
  print(colors[current] .. ' ' .. current .. '/' .. #colors)
end

vim.keymap.set('n', '>', next_color)
vim.keymap.set('n', '<', prev_color)
