#for I in $(seq 7) ;  do  scp nikos/hadoop_install/install.sh slave$I: ;  done

sudo sed -i '2,$s/^/#/' /etc/hosts
sudo sh -c "cat >> /etc/hosts << EOF
83.212.117.16   master
83.212.117.22   slave1
83.212.117.34   slave2
83.212.117.35   slave3
83.212.117.45   slave4
83.212.117.47   slave5
83.212.117.48   slave6
83.212.118.241  slave7
EOF"

scp master:.ssh/id_rsa .ssh
ssh master
ssh localhost


cat >> .bashrc << EOF
# HADOOP-RELATED #
export HD_HOME=/usr/local/hd
export JAVA_HOME=/usr/lib/jvm/java-7-openjdk-amd64/jre
export PATH="\$PATH:/usr/local/hd/bin:/usr/local/hd/sbin"
EOF


scp master:nikos/hadoop-2.7.1.tar.gz .
scp master:nikos/hadoop_install/hadoop.patch .

cd /usr/local/
sudo tar xzf ~/hadoop-2.7.1.tar.gz 
\rm ~/hadoop-2.7.1.tar.gz 

sudo chown -R user:user hadoop-2.7.1/
sudo ln -snf hadoop-2.7.1 hd

cd hd
patch -p1 -i ~/hadoop.patch
\rm ~/hadoop.patch

cd
sudo mkdir -p /app/hd/tmp
sudo chmod 777 /app/hd/tmp

#\rm -r /app/hd/tmp/*
#hdfs namenode -format
#start-dfs.sh
#start-yarn.sh
#hadoop fs -mkdir -p /user/user


sudo sh -c "cat >> /etc/sysctl.conf << EOF
# HADOOP-RELATED #
net.ipv6.conf.all.disable_ipv6 = 1
net.ipv6.conf.default.disable_ipv6 = 1
net.ipv6.conf.lo.disable_ipv6 = 1
EOF"
sudo service networking restart


#sudo apt-get install openjdk-7-jdk
