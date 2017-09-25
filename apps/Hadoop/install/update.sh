scp master:nikos/hadoop-2.7.2.tar.gz .
scp master:nikos/naxos/hadoop/install/hadoop.patch .

cd /usr/local/
sudo \rm -r hadoop-2.7.1/

sudo tar xzf ~/hadoop-2.7.2.tar.gz
\rm ~/hadoop-2.7.2.tar.gz

sudo chown -R user:user hadoop-2.7.2/
sudo ln -snf hadoop-2.7.2 hd

cd hd
patch -p1 -i ~/hadoop.patch
\rm ~/hadoop.patch

\rm ~/update.sh
