## initializes the dev environment

## install ultimate vim - awesome ed.
git clone --depth=1 https://github.com/amix/vimrc.git ~/.vim_runtime
sh ~/.vim_runtime/install_awesome_vimrc.sh

## copy vim settings
cp .vimrc ~/
cp my_configs.vim ~/.vim_runtime/

echo "done"
