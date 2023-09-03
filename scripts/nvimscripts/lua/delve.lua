local M = {}

M.id = 0
vim.api.nvim_call_function("sign_define", {"gobreakpoint", {text=">>"}})

function M.add_breakpoint()
  local cur_buff = vim.api.nvim_win_get_buf(0)
  local buf_name = vim.api.nvim_buf_get_name(cur_buff)
  local line_num, _ = unpack(vim.api.nvim_win_get_cursor(0))

  vim.api.nvim_command("sign place " .. M.id+1 .. " line=" .. line_num .. " name=gobreakpoint file=" .. buf_name)
  M.id = M.id + 1
end

function M.remove_breakpoint()
  local cur_buff = vim.api.nvim_win_get_buf(0)
  local buf_name = vim.api.nvim_buf_get_name(cur_buff)
  local line_num, _ = unpack(vim.api.nvim_win_get_cursor(0))
  local placed = vim.api.nvim_call_function("sign_getplaced", {cur_buff})

  local breakpoint = nil

  for i in ipairs(placed) do
    local x = placed[i]
    local exit = false

    if x.bufnr == cur_buff then
      for j in ipairs(x.signs) do
        local y = x.signs[j]

        if y.name == 'gobreakpoint' and y.lnum == line_num then
          breakpoint = y
          exit = true
          break
        end
      end
    end

    if exit then
      break
    end
  end

  vim.api.nvim_call_function("sign_unplace", {"", {buffer=cur_buff, id=breakpoint.id}})
end

function M.show_breakpoints()
  local cur_buff = vim.api.nvim_win_get_buf(0)
  local buf_name = vim.api.nvim_buf_get_name(cur_buff)
  local line_num, _ = unpack(vim.api.nvim_win_get_cursor(0))
  local placed = vim.api.nvim_call_function("sign_getplaced", {cur_buff})

  for i in ipairs(placed) do
    local x = placed[i]
    local exit = false

    if x.bufnr == cur_buff then
      exit = true
      local list = {}


      for j in ipairs(x.signs) do
        local y = x.signs[j]

        if y.name == "gobreakpoint" then
          table.insert(list, {bufnr=cur_buff, lnum=y.lnum})
        end
      end

      vim.api.nvim_call_function("setqflist", {list, "r"})
      vim.api.nvim_command("copen")
    end

    if exit then
      break
    end
  end
end

return M
