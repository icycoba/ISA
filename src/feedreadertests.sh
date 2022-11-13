# Autor: Martin Hlinský
# Projekt: Síťové aplikace a správa sítí - Čtečka novinek ve formátu Atom a RSS s podporou TLS
# Soubor: feedreadertests.sh

# Pro správnou funkčnost skriptu je potřeba mít dostupné požadované certifikáty ve složce /etc/ssl/certs
successcount=0;
count=0;

#0
./feedreader -f ../tests/feedfile.txt -C /etc/ssl/certs -c /etc/ssl/certs/GTS_Root_R1.pem -Tau >/dev/null 2>/dev/null
retval=$?
if [ $retval -eq 0 ]; then
    successcount=$((successcount+1));
else echo "Test $count byl neúspěšný";
fi
count=$((count+1));
./feedreader -f ../tests/feedfile -C /etc/ssl/certs -Tau >/dev/null 2>/dev/null
retval=$?
if [ $retval -eq 0 ]; then
    successcount=$((successcount+1));
else echo "Test $count byl neúspěšný";
fi
count=$((count+1));
./feedreader http://www.fit.vutbr.cz/news/news-rss.php >/dev/null 2>/dev/null
retval=$?
if [ $retval -eq 0 ]; then
    successcount=$((successcount+1));
else echo "Test $count byl neúspěšný";
fi
count=$((count+1));

#1
./feedreader -f ../tests/feedfile.txt "http://www.fit.vutbr.cz/news/news-rss.php" -C /etc/ssl/certs -c /etc/ssl/certs/GTS_Root_R1.pem -Tau >/dev/null 2>/dev/null
retval=$?
if [ $retval -eq 1 ]; then
    successcount=$((successcount+1));
else echo "Test $count byl neúspěšný";
fi
count=$((count+1));
./feedreader "http://www.fit.vutbr.cz/news/news-rss.php" -f ../tests/feedfile.txt -C /etc/ssl/certs -c /etc/ssl/certs/GTS_Root_R1.pem -Tau >/dev/null 2>/dev/null
retval=$?
if [ $retval -eq 1 ]; then
    successcount=$((successcount+1));
else echo "Test $count byl neúspěšný";
fi
count=$((count+1));
./feedreader >/dev/null 2>/dev/null
retval=$?
if [ $retval -eq 1 ]; then
    successcount=$((successcount+1));
else echo "Test $count byl neúspěšný";
fi
count=$((count+1));
./feedreader -h >/dev/null 2>/dev/null
retval=$?
if [ $retval -eq 1 ]; then
    successcount=$((successcount+1));
else echo "Test $count byl neúspěšný";
fi
count=$((count+1));
./feedreader "http://www.fit.vutbr.cz/news/news-rss.php" -C /etc/ssl/certs -c /etc/ssl/certs/GTS_Root_R1.pem -Tau -h >/dev/null 2>/dev/null
retval=$?
if [ $retval -eq 1 ]; then
    successcount=$((successcount+1));
else echo "Test $count byl neúspěšný";
fi
count=$((count+1));

#0
./feedreader -u "http://www.fit.vutbr.cz/news/news-rss.php" -C /etc/ssl/certs -a -c /etc/ssl/certs/GTS_Root_R1.pem -T >/dev/null 2>/dev/null
retval=$?
if [ $retval -eq 0 ]; then
    successcount=$((successcount+1));
else echo "Test $count byl neúspěšný";
fi
count=$((count+1));
./feedreader -C /etc/ssl/certs -u "http://www.fit.vutbr.cz/news/news-rss.php" >/dev/null 2>/dev/null
retval=$?
if [ $retval -eq 0 ]; then
    successcount=$((successcount+1));
else echo "Test $count byl neúspěšný";
fi
count=$((count+1));
#1
./feedreader -C /etc/ssl/certs -u "http://www.fit.vutbr.cz/news/news-rss.php" -TT >/dev/null 2>/dev/null
retval=$?
if [ $retval -eq 1 ]; then
    successcount=$((successcount+1));
else echo "Test $count byl neúspěšný";
fi
count=$((count+1));

#0
./feedreader -C /etc/ssl/certs "http://www.fit.vutbr.cz/news/news-rss.php" >/dev/null 2>/dev/null
retval=$?
if [ $retval -eq 0 ]; then
    successcount=$((successcount+1));
else echo "Test $count byl neúspěšný";
fi
count=$((count+1));
./feedreader -C /etc/ssl/certs 'http://www.fit.vutbr.cz/news/news-rss.php' >/dev/null 2>/dev/null
retval=$?
if [ $retval -eq 0 ]; then
    successcount=$((successcount+1));
else echo "Test $count byl neúspěšný";
fi
count=$((count+1));
./feedreader -C /etc/ssl/certs http://www.fit.vutbr.cz/news/news-rss.php >/dev/null 2>/dev/null
retval=$?
if [ $retval -eq 0 ]; then
    successcount=$((successcount+1));
else echo "Test $count byl neúspěšný";
fi

count=$((count+1));

echo "$successcount/$count testů bylo úspěšných!";