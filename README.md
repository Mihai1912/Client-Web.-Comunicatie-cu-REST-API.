# PROTOCOALE DE COMUNICAŢIE : Tema 4 Client Web. Comunicaţie cu REST API.

### Popescu Mihai-Costel 324 CD

In implemetarea acestei teme m-am folosit de laboratorul 9, iar pentru 
lucru cu obiecte de tip JSON am ales varianta sugerata in cerinta si 
anume biblioteca nlohmann deoarece implementarea temei este in C++.

Am inceput prin declarare unor variabile in care voi tine minte
cookie-ul de login, token-ul JWT, si socket-ul prin care clientul 
comunica cu serverul

Programul cicleaza intr-un ``` while(1) ``` si citeste si interpreteaza
comenzi pana la intalnirea comenzii exit.

* Descriere pe scurt a comenzilor :
    * ``` register ``` verific daca exista deja un user logat, daca
    da se afiseaza o eroare, iar daca nu se incepe citirea
    credentialelor, verific daca acestea contin spatii, daca da user-ul
    este informat ca acestea contin spatii si acesta poate introduce 
    iar credentialele. Creez obiectul JSON cu ajutorul bibliotecii 
    nlohmann care va contine username-ul si parola introdusa de user, 
    compun mesajul care va fi trimis catre server acesta fiind de tip 
    ``` post ```, fac conexiunea cu serverul, ii trimit mesajul creat, 
    apoi iau raspunsul serverului iar, daca acesta contine string-ul 
    "error" inseamna ca trebuie afisata o eroare daca nu user-ul este 
    adaugat cu succes, iar in final inchid conexiunea cu serverul.
    * ``` login ``` este asemanatoare cu ``` register ``` doar ca in 
    cazul in care serverul nu intoarce o eroare, din raspunsul 
    serverului extrag cookie-ul care dovedeste logarea user-ului si il 
    tin minte in variabila declarata la inceputul programului.
    * ``` enter_library ``` verific daca user-ul este logat, daca nu 
    cumva este deja in librarie, compun mesajul care il voi trimite la 
    server, deschid conexiunea cu serverul, ii trimit mesajul si 
    interpretez raspunsul lui, daca acesta contine string-ul error 
    afisez o eroare daca nu extrag token-ul JWT care devedeste accesul 
    la librarie si il retin in variabila declarata mai sus.
    * ``` get_books ``` verific daca user-ul este logat, daca are acces 
    la librarie, compun mesajul care il voi trimite la server, deschid 
    conexiunea cu serverul, ii trimit mesajul si interpretez raspunsul 
    lui,daca acesta contine string-ul error afisez o eroare daca nu 
    extrag lista de obiecte JSON din raspuns si o parsez cu ajutorul 
    functiei parse din biblioteca nlohmann, si printez titlul si id-ul 
    fiecarei carti
    * ``` get_book ``` verific daca user-ul este logat, daca are acces 
    la librarie, citesc si verific daca id-ul este numar, iar apoi 
    compun mesajul care il voi trimite la server, deschid conexiunea cu 
    serverul, ii trimit mesajul si interpretez raspunsul lui,daca 
    acesta contine string-ul error afisez o eroare daca nu extrag 
    obiectul JSON si il parsez cu ajutorul fuctiei parse din biblioteca 
    nlohmann, dupa care printez datele cartii.
    * ``` add_book ``` verific daca user-ul este logat, daca are acces 
    la librarie, citesc datele cartii verific daca page_count este 
    numar, creez obiectul JSON, compun mesajul care il voi trimite la 
    server, deschid conexiunea cu serverul, ii trimit mesajul si 
    interpretez raspunsul lui,daca acesta contine string-ul error 
    afisez o eroare daca nu cartea a fost adaugata cu succes.
    * ``` delete_book ``` verific daca user-ul este logat, daca are 
    acces la librarie, citesc id-ul si verific daca acesta este 
    numar, , compun mesajul care il voi trimite la server, deschid 
    conexiunea cu serverul, ii trimit mesajul si interpretez raspunsul 
    lui,daca acesta contine string-ul error afisez o eroare daca nu 
    cartea a fost stearsa cu succes.
    * ``` logout ``` verific daca user-ul este logat, compun mesajul 
    care il voi trimite la server, deschid conexiunea cu serverul, ii 
    trimit mesajul si interpretez raspunsul lui,daca acesta contine 
    string-ul error afisez o eroare daca nu, eliberez variabilele ce 
    contin cookie-ul de logare si token-ul JWT.