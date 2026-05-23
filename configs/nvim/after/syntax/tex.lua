-- make amsmath environments traditional tex math enviroments.
vim.api.nvim_call_function('TexNewMathZone', {'E', 'align', 1})
vim.api.nvim_call_function('TexNewMathZone', {'F', 'multline', 1})
vim.api.nvim_call_function('TexNewMathZone', {'H', 'equation', 1})
vim.api.nvim_call_function('TexNewMathZone', {'I', 'gather', 1})

-- make mathpar package environment lint as mathmode.
vim.api.nvim_call_function('TexNewMathZone', {'I', 'mathpar', 1})

-- spellcheck the toplevel syntactical entity, that is outside the begin document.
vim.api.nvim_command('syntax spell toplevel')

-- parsing always starts at least this many lines backwards.
vim.api.nvim_command('syntax sync minlines=50')

-- parsing always stops after this many lines forward from minlines.
vim.api.nvim_command('syntax sync maxlines=500')

-- create syntax regions for latex minted inline and blocks.
vim.api.nvim_command('syntax region texMintedZone matchgroup=texStatement start="\\\\begin{minted}\\(\\[[^]]*\\]\\)\\?{[^}]*}" end="\\\\end{minted}" keepend contains=NONE contained containedin=ALL')
vim.api.nvim_command('syntax region texMintInlineZone matchgroup=texStatement start="\\\\mintinline\\(\\[[^]]*\\]\\)\\?{[^}]*}{" end="}" keepend contains=NONE contained containedin=ALL')

-- define how the text inside the minted regions should look.
vim.api.nvim_command('highlight default link texMintedZone Normal')
vim.api.nvim_command('highlight default link texMintInlineZone Normal')
