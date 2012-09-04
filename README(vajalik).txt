Klient:
Klient võib töötada mitmes reşiimis, reşiimi valik oleneb käsurealt antavatest parameetritest. Järgnevalt toon mõned näited.

GuessingClient [IP] [bot/suvaline] [nimi]

GuessingClient 127.0.0.1 - ühendutakse serverisse ja arvu hakkab ära arvama kasutaja. (originaalserver)

GuessingClient 127.0.0.1 bot - ühendutakse serverisse ja arvu hakkab ära arvama bot. (originaalserver)

GuessingClient 127.0.0.1 suvaline Toomas - ühendutakse serverisse ja arvu hakkab ära arvama kasutaja ning serverisse saadetakse nimi(Toomas). (modifitseeritud server)

GuessingClient 127.0.0.1 bot Robot - ühendutakse serverisse ja arvu hakkab ära arvama bot ning serverisse saadetakse nimi(Robot). (modifitseeritud server)

NB! Nime andmisel(kolmas parameeter) eeldab klient, et serverilt tuleb ka highscore tabel(seega jäädakse ootama ja sellisel moel pole mõtet klienti tavalisse serverisse ühendada). Tavalisse serverisse ühendumisel ei tohiks anda kolmandat parameetrit.


Server:
Server teenindab klienti, mis saadab introductionis oma nime - tagasi saadetakse vastus ja highscore tabel. Seega tuleks teise lisaülesande testimisel kindlasti kliendi käivitamisel anda ette kolm parameetrit. Tavaülesande ja esimese lisaülesande testimiseks sobib originaalserver.

Server kasutab mängija IP asemel nime, kui vastav mängija esineb struktuuris 'players' ja nimi on olemas. Seega, kui mängija on juba kustutatud, siis kasutatakse väljundis IP-d (ma ei hakanud olemasoleva koodi struktuuri oluliselt muutma).


