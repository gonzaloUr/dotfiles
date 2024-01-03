-- Make amsmath environments traditional tex math enviroments
vim.api.nvim_call_function('TexNewMathZone', {'E', 'align', 1})
vim.api.nvim_call_function('TexNewMathZone', {'F', 'multline', 1})
vim.api.nvim_call_function('TexNewMathZone', {'H', 'equation', 1})
vim.api.nvim_call_function('TexNewMathZone', {'I', 'gather', 1})

-- spellcheck the toplevel syntactical entity (outside begin document)
vim.api.nvim_command('syntax spell toplevel')

-- parsing always starts at least this many lines backwards
vim.api.nvim_command('syntax sync minlines=50')

-- parsing always stops after this many lines forward from minlines
vim.api.nvim_command('syntax sync maxlines=500')
