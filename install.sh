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

ln -s $GTK_GOAGENT_PATH/gtk_goagent /usr/bin/gtk_goagent


echo -e "#Python Path\n\n" > $HOME/.gtk_goagentrc
echo -e "python_path `whereis python | awk '{print $2}'`\n\n" >> $HOME/.gtk_goagentrc 
echo -e "#GoAgent Path\n\n" >> $HOME/.gtk_goagentrc 
echo -e "goagent_path \n\n" >> $HOME/.gtk_goagentrc 
echo -e "#Language\n\n" >> $HOME/.gtk_goagentrc 
echo -e "language_env zh_CN.UTF-8\n\n" >> $HOME/.gtk_goagentrc 
echo -e "#Gtk GoAgent Path\n\n"
echo -e "gtk_goagent_path $GTK_GOAGENT_PATH\n\n" >> $HOME/.gtk_goagentrc 
echo -e "#Font\n\n"
echo -e "font AR PL KaitiM GB Bold 13\n\n" >> $HOME/.gtk_goagentrc 
echo -e "#GoAgent Auto Upgrade\n\n"
echo -e "goagent_auto_upgrade false\n\n" >> $HOME/.gtk_goagentrc 
echo '#End Of Gtk GoAgent Config File' >> $HOME/.gtk_goagentrc 

exit 0
