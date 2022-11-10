# Autor: Martin Hlinský
# Projekt: Síťové aplikace a správa sítí - Čtečka novinek ve formátu Atom a RSS s podporou TLS
# Soubor: feedreadertest.sh

DIR=../tests/testOut

if [ -d "$DIR" ];
then
    rm -rf $DIR;
fi

mkdir $DIR;

#0
./feedreader -f ../tests/feedfile.txt -C /etc/ssl/certs -c /etc/ssl/certs/GTS_Root_R1.pem -Tau
./feedreader -f ../tests/feedfile.txt -C /etc/ssl/certs -Tau
#should ret zero, dunno why doesnt
./feedreader -f ../tests/feedfile.txt -c /etc/ssl/certs/GTS_Root_R1.pem -Tau

#1
./feedreader -f ../tests/feedfile.txt "http://www.fit.vutbr.cz/news/news-rss.php" -C /etc/ssl/certs -c /etc/ssl/certs/GTS_Root_R1.pem -Tau
./feedreader "http://www.fit.vutbr.cz/news/news-rss.php" -f ../tests/feedfile.txt -C /etc/ssl/certs -c /etc/ssl/certs/GTS_Root_R1.pem -Tau
./feedreader
./feedreader -h
./feedreader "http://www.fit.vutbr.cz/news/news-rss.php" -C /etc/ssl/certs -c /etc/ssl/certs/GTS_Root_R1.pem -Tau -h
#0
./feedreader -u "http://www.fit.vutbr.cz/news/news-rss.php" -C /etc/ssl/certs -a -c /etc/ssl/certs/GTS_Root_R1.pem -T
./feedreader -C /etc/ssl/certs -u "http://www.fit.vutbr.cz/news/news-rss.php"
#1
./feedreader -C /etc/ssl/certs -u "http://www.fit.vutbr.cz/news/news-rss.php" -TT

#0
./feedreader -C /etc/ssl/certs "http://www.fit.vutbr.cz/news/news-rss.php"
./feedreader -C /etc/ssl/certs 'http://www.fit.vutbr.cz/news/news-rss.php'
./feedreader -C /etc/ssl/certs http://www.fit.vutbr.cz/news/news-rss.php