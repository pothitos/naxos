scp master:nikos/hadoop-2.7.0.tar.gz .
scp master:nikos/hadoop_install/hadoop.patch .

cd /usr/local/
sudo \rm -r hadoop-2.6.0/

sudo tar xzf ~/hadoop-2.7.0.tar.gz 
\rm ~/hadoop-2.7.0.tar.gz 

sudo chown -R user:user hadoop-2.7.0/
sudo ln -snf hadoop-2.7.0 hd

cd hd
patch -p1 -i ~/hadoop.patch
\rm ~/hadoop.patch

\rm ~/update.sh
