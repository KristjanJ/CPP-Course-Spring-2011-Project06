Klient:
Klient v�ib t��tada mitmes re�iimis, re�iimi valik oleneb k�surealt antavatest parameetritest. J�rgnevalt toon m�ned n�ited.

GuessingClient [IP] [bot/suvaline] [nimi]

GuessingClient 127.0.0.1 - �hendutakse serverisse ja arvu hakkab �ra arvama kasutaja. (originaalserver)

GuessingClient 127.0.0.1 bot - �hendutakse serverisse ja arvu hakkab �ra arvama bot. (originaalserver)

GuessingClient 127.0.0.1 suvaline Toomas - �hendutakse serverisse ja arvu hakkab �ra arvama kasutaja ning serverisse saadetakse nimi(Toomas). (modifitseeritud server)

GuessingClient 127.0.0.1 bot Robot - �hendutakse serverisse ja arvu hakkab �ra arvama bot ning serverisse saadetakse nimi(Robot). (modifitseeritud server)

NB! Nime andmisel(kolmas parameeter) eeldab klient, et serverilt tuleb ka highscore tabel(seega j��dakse ootama ja sellisel moel pole m�tet klienti tavalisse serverisse �hendada). Tavalisse serverisse �hendumisel ei tohiks anda kolmandat parameetrit.


Server:
Server teenindab klienti, mis saadab introductionis oma nime - tagasi saadetakse vastus ja highscore tabel. Seega tuleks teise lisa�lesande testimisel kindlasti kliendi k�ivitamisel anda ette kolm parameetrit. Tava�lesande ja esimese lisa�lesande testimiseks sobib originaalserver.

Server kasutab m�ngija IP asemel nime, kui vastav m�ngija esineb struktuuris 'players' ja nimi on olemas. Seega, kui m�ngija on juba kustutatud, siis kasutatakse v�ljundis IP-d (ma ei hakanud olemasoleva koodi struktuuri oluliselt muutma).


