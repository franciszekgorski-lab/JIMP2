call plug#begin('~/.vim/plugged')
Plug 'dense-analysis/ale'
call plug#end()

let g:ale_linters = {'c': ['gcc']}
let g:ale_c_include_dirs = ['headers']
let g:ale_fix_on_save = 1
