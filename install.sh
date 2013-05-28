#!/bin/bash 

GTK_GOAGENT_PATH='/usr/local/share/gtk_goagent'

mkdir $GTK_GOAGENT_PATH
cp gtk_goagent $GTK_GOAGENT_PATH
cp pre_ui $GTK_GOAGENT_PATH
cp upgrade $GTK_GOAGENT_PATH
cp upload $GTK_GOAGENT_PATH
cp -R locale $GTK_GOAGENT_PATH
cp -R po $GTK_GOAGENT_PATH
cp -R img $GTK_GOAGENT_PATH


echo '#Python Path' > $HOME/.gtk_goagentrc
echo 'python_path '`whereis python | awk '{print $2}'` >> $HOME/.gtk_goagentrc 
echo '#GoAgent Path' >> $HOME/.gtk_goagentrc 
echo 'goagent_path ' >> $HOME/.gtk_goagentrc 
echo '#Language' >> $HOME/.gtk_goagentrc 
echo 'language_env zh_CN.UTF-8' >> $HOME/.gtk_goagentrc 
echo '#Gtk GoAgent Path'
echo $GTK_GOAGENT_PATH >> $HOME/.gtk_goagentrc 
echo '#Font'
echo 'font AR PL KaitiM GB Bold 13' >> $HOME/.gtk_goagentrc 
echo '#GoAgent Auto Upgrade'
echo 'goagent_auto_upgrade false' >> $HOME/.gtk_goagentrc 
echo '#End Of Gtk GoAgent Config File' >> $HOME/.gtk_goagentrc 

exit 0
