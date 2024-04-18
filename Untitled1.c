#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define MAX_PRODUITS 255
#define MAX_VENTES 255
#define MAX_FOURNISSEURS 255
#define MAX_CLIENTS 255
void go_home(); //proto de go_home
void accueil_client(bool connection_state);

struct Client {
    int id;
    char nom[50];
    char mail[50];
    char pw[50];
    int nb_achats;
};

struct Fournisseur {
    int id;
    char name[50];
    char pw[50];
    int nb_ventes;
};

struct Produit {
    int id;
    char name[50];
    char desc[50];
    int price;
    int avail_stock;
    int nb_ventes;
};

struct Vente {
    int id;
    char product_name[50];
    int quantity;
    int amount;
    char client_name[50];
};


void cleanBuffer() { // Fonction pour nettoyer le tampon d'entrée (a cause de fgets/scanf bug)
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }

    return;
}

void ajouter_produit() {
    struct Produit p;
    FILE *file;

    printf("\nPour creer un produit, veuillez renseigner certaines infos :");
    printf("\nID : ");
    scanf("%d", &p.id);
    printf("Titre : ");
    cleanBuffer();
    fgets(p.name, sizeof(p.name), stdin);
    p.name[strcspn(p.name, "\n")] = 0; // Supprime le retour à la ligne
    printf("Description : ");
    fgets(p.desc, sizeof(p.desc), stdin);
    p.desc[strcspn(p.desc, "\n")] = 0; // Supprime le retour à la ligne
    printf("Prix : ");
    scanf("%d", &p.price);
    printf("Stock disponible : ");
    scanf("%d", &p.avail_stock);
    p.nb_ventes = 0;

    file = fopen("produits.dat", "ab"); //append binary pour créer auto

    if(file == NULL) {
        perror("Erreur lors de l'ouverture du fichier.");
    }

    fwrite(&p, sizeof(struct Produit), 1, file);
    fclose(file);

    printf("\nLe produit a bien ete cree.\n");
    go_home();
}

void modifier_produit() {
    int toedit_id;
    struct Produit p;
    FILE *file, *tempFile;
    bool edited = false;
    char edited_product_name[50];

    printf("\nPour modifier un produit, renseignez son ID : ");
    scanf("%d", &toedit_id);

    file = fopen("produits.dat", "rb");
    if(file == NULL) {
        perror("Erreur lors de l'ouverture du fichier.");
        return;
    }

    tempFile = fopen("temp.dat", "wb");
    if(tempFile == NULL) {
        perror("Erreur lors de la création du fichier temp.");
        fclose(file);
        return;
    }

    while(fread(&p, sizeof(struct Produit), 1, file)) {
        if(p.id == toedit_id) {
            printf("Modification du produit No.%d (%s) :\n", toedit_id, p.name);

            printf("Nom du Produit : ");
            cleanBuffer();
            fgets(p.name, sizeof(p.name), stdin);
            p.name[strcspn(p.name, "\n")] = 0; // Supprime le retour à la ligne

            printf("Description : ");
            fgets(p.desc, sizeof(p.desc), stdin);
            p.desc[strcspn(p.desc, "\n")] = 0; // Supprime le retour à la ligne

            printf("Prix : ");
            scanf("%d", &p.price);
            printf("Stock dispo : ");
            scanf("%d", &p.avail_stock);

            strcpy(edited_product_name, p.name);
            edited = true;
        }
        fwrite(&p, sizeof(struct Produit), 1, tempFile);
    }

    fclose(file);
    fclose(tempFile);

    remove("produits.dat");
    rename("temp.dat", "produits.dat");

    if(edited == true)
        printf("\nLe produit No.%d (%s) a bien ete modifie.\n", toedit_id, edited_product_name);
    else
        printf("\nLe produit est introuvable.\n");

    go_home();
}

void supprimer_produit() {
    int todel_id;
    struct Produit p;
    FILE *file, *tempFile;
    bool deleted = false;
    char deleted_product_name[50];

    printf("\nPour supprimer un produit, renseignez son ID : ");
    scanf("%d", &todel_id);

    file = fopen("produits.dat", "rb");
    if(file == NULL) {
        perror("Erreur lors de l'ouverture du fichier.");
        return;
    }

    tempFile = fopen("temp.dat", "wb");
    if(tempFile == NULL) {
        perror("Erreur lors de la création du fichier temp.");
        fclose(file);
        return;
    }

    while(fread(&p, sizeof(struct Produit), 1, file)) {
        if(p.id != todel_id) {
            fwrite(&p, sizeof(struct Produit), 1, tempFile);
        }
        else {
            strcpy(deleted_product_name, p.name);
            deleted = true;
        }
    }

    fclose(file);
    fclose(tempFile);

    remove("produits.dat");
    rename("temp.dat", "produits.dat");

    if(deleted == true)
        printf("\nLe produit %d (%s) a bien ete supprime.\n", todel_id, deleted_product_name);
    else
        printf("\nLe produit est introuvable.\n");

    go_home();
}

void consulter_produit() {
    int id_produit = 0;
    int produit_i = 0;
    struct Produit found_produit;
    struct Produit tableau_produits[MAX_PRODUITS];
    struct Produit p;
    FILE *file;
    file = fopen("produits.dat", "rb");

    if(file == NULL) {
        perror("Aucun produit actuellement.");
    }
    else {
        printf("\nListe des produits :\n");

        while(fread(&p, sizeof(struct Produit), 1, file) && produit_i < MAX_PRODUITS) {
            printf("%d : %s\n", p.id, p.name);
            tableau_produits[produit_i] = p;
            produit_i++;
        }
        fclose(file);

        printf("\nRenseignez l'id du produit a consulter : ");
        scanf("%d", &id_produit);

        for(int i = 0;i < produit_i;i++) {
            if(id_produit == tableau_produits[i].id) {
                found_produit = tableau_produits[i];
            }
        }

        if(id_produit == found_produit.id) {
            printf("\n\nID : %d", found_produit.id);
            printf("\nProduit : %s", found_produit.name);
            printf("\nDescription : %s", found_produit.desc);
            printf("\nPrix : %d $", found_produit.price);
            printf("\nStock dispo : %d", found_produit.avail_stock);
            printf("\nNb Ventes : %d\n", found_produit.nb_ventes);
        }
        else {
            printf("\nProduit introuvable !\n");
        }
    }

    go_home();
}

void acheter_produit() {
    int produit_id;
    FILE *connected_usr = fopen("logged.dat", "rb");
    FILE *fichier_produits = fopen("produits.dat", "rb+");
    FILE *fichier_ventes = fopen("ventes.dat", "ab");
    FILE *fichier_clients = fopen("clients.dat", "ab");
    struct Produit p;
    struct Client c;
    bool produit_found = false;
    int quantity = 1; //quantité a 1 de base
    char client_name[50];

    if (connected_usr == NULL) {
        perror("Vous devez etre connecte");
        go_home();
    }
    else {
        while(fread(&c, sizeof(struct Client), 1, connected_usr)) {
            strcpy(client_name, c.nom);
        }
    }

    printf("Entrez l'ID du produit que vous souhaitez acheter : ");
    scanf("%d", &produit_id);
    printf("Quantite : ");
    scanf("%d", &quantity);

    if (fichier_produits == NULL || fichier_ventes == NULL) {
        perror("Erreur lors de l'ouverture d'un fichier");
        return;
    }

    //On actualise produits
    while (fread(&p, sizeof(struct Produit), 1, fichier_produits)) {
        if (p.id == produit_id && p.avail_stock >= quantity) {
            produit_found = true;
            p.avail_stock -= quantity; // Reduction du stock
            p.nb_ventes += quantity; // Incrémentation du nombre de ventes
            fseek(fichier_produits, -(long)sizeof(struct Produit), SEEK_CUR); // Retour a la position du produit lu
            fwrite(&p, sizeof(struct Produit), 1, fichier_produits); // Mise a jour du produit dans le fichier
            break;
        }
    }

    if (produit_found) {
        struct Vente nouvelleVente = {
            .id = produit_id, // Utiliser un ID unique pour chaque vente pourrait être une meilleure approche
            .quantity = quantity,
            .amount = p.price * quantity
        };
        strcpy(nouvelleVente.product_name, p.name);
        strcpy(nouvelleVente.client_name, client_name);

        fwrite(&nouvelleVente, sizeof(struct Vente), 1, fichier_ventes);

        //On actualise clients
        while (fread(&c, sizeof(struct Client), 1, fichier_clients)) {
            if (c.nom == client_name) {
                c.nb_achats += quantity; // Incrémentation du nombre dachats
                fseek(fichier_clients, -(long)sizeof(struct Client), SEEK_CUR); // Retour a la position du produit lu
                fwrite(&c, sizeof(struct Client), 1, fichier_clients); // Mise a jour du client avec le nouveau nb_achats
                break;
            }
        }

        printf("\n\nProduit achete avec succes !\n");
        printf("ID : %d\n", p.id);
        printf("Nom : %s\n", p.name);
        printf("Prix : %d $\n", p.price);
        printf("Quantite : %d\n", quantity);
        printf("Total paye : %d $\n", quantity * p.price);
    } else {
        printf("Produit non trouve ou stock insuffisant.\n");
    }

    fclose(fichier_produits);
    fclose(fichier_ventes);
    go_home();
}

void historique_ventes() {
    int id_vente = 0;
    int vente_i = 0;
    struct Vente found_vente;
    struct Vente tableau_ventes[MAX_VENTES];
    struct Vente v;
    FILE *file;
    file = fopen("ventes.dat", "rb");

    if(file == NULL) {
        perror("Aucune vente actuellement.");
    }
    else {
        printf("\nHistorique des ventes :");
        printf("\n_______________________\n");

        while(fread(&v, sizeof(struct Vente), 1, file) && vente_i < MAX_VENTES) {
            char edited_vente_name[100];
            strcpy(edited_vente_name, v.client_name);
            printf("%d : %s\nLot de %d achete par %s pour un total de %d $\n\n", v.id, v.product_name, v.quantity, edited_vente_name, v.amount);
            tableau_ventes[vente_i] = v;
            vente_i++;
        }
        fclose(file);
    }

    go_home();
}

int check_connected(char *mail, char *pw) { //retourne 0 si connected sinon 1
    FILE *file; //Les clients incsrits
    FILE *file_connected; //Le client connecte
    struct Client c;
    int trouve = 0;
    int connected = 0;

    file = fopen("clients.dat", "rb");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier.");
        return 1;
    }

    while(fread(&c, sizeof(struct Client), 1, file)) {
        if(strcmp(c.mail, mail) == 0 && strcmp(c.pw, pw) == 0) {
            trouve = 1;

            file_connected = fopen("logged.dat", "ab");
            if(file_connected == NULL) {
                fclose(file_connected);
            }
            fwrite(&c, sizeof(struct Client), 1, file_connected);
            fclose(file_connected);
            connected = 1;
            break;
        }
    }
    fclose(file);

    if(trouve && connected) {
        return 0;
    }
    else {
        return 1;
    }
}

void connexion() { //Menu du choix Connexion/Inscription
    printf("Pour vous connecter, veuillez saisir ces infos :\n");
    char login_mail[100];
    char login_pw[100];

    printf("\nEmail : ");
    scanf("%s", &login_mail);
    printf("Mot de passe : ");
    scanf("%s", &login_pw);

    if(check_connected(login_mail, login_pw) == 0) {
        accueil_client(true);
    } else {
        int retry_choice = 0;

        printf("\nEmail ou mot de passe incorrect.\n");
        printf("\na : Reessayer,     any : Quitter.\n");
        scanf(" %c", &retry_choice);

        if(retry_choice == 'a')
            connexion();
        else
            go_home();
    }
}

void ajouter_client() {
    struct Client c;
    FILE *file;

    printf("\nPour creer un compte, renseignez les infos suivantes : ");
    printf("\nID : ");
    scanf("%d", &c.id);
    printf("\nVotre nom complet : ");
    cleanBuffer();
    fgets(c.nom, sizeof(c.nom), stdin);
    c.nom[strcspn(c.nom, "\n")] = 0; // Supprime le retour à la ligne
    printf("Votre e-mail : ");
    fgets(c.mail, sizeof(c.mail), stdin);
    c.mail[strcspn(c.mail, "\n")] = 0; // Supprime le retour à la ligne
    printf("Votre password : ");
    fgets(c.pw, sizeof(c.pw), stdin);
    c.pw[strcspn(c.pw, "\n")] = 0;
    c.nb_achats = 0;

    file = fopen("clients.dat", "ab"); //append binary pour créer auto

    if(file == NULL) {
        perror("Erreur lors de l'ouverture du fichier.");
    }

    fwrite(&c, sizeof(struct Client), 1, file);
    fclose(file);

    printf("\nVotre profil a bien ete cree.\n");
    connexion();
}

void consulter_client() {
    int id_client = 0;
    int client_i = 0;
    struct Client found_client;
    struct Client tableau_clients[MAX_CLIENTS];
    struct Client c;
    FILE *file;
    file = fopen("clients.dat", "rb");

    if(file == NULL) {
        perror("Aucun client actuellement.\n");
    }
    else {
        printf("\nListe des clients :\n");

        while(fread(&c, sizeof(struct Client), 1, file) && client_i < MAX_CLIENTS) {
            printf("%d : %s\n", c.id, c.nom);
            tableau_clients[client_i] = c;
            client_i++;
        }
        fclose(file);

        printf("\nRenseignez l'ID du client a consulter : ");
        scanf("%d", &id_client);

        for(int i = 0;i < client_i;i++) {
            if(id_client== tableau_clients[i].id) {
                found_client = tableau_clients[i];
            }
        }

        if(id_client == found_client.id) {
            printf("\n\nID : %d", found_client.id);
            printf("\nNom : %s", found_client.nom);
            printf("\nAdresse Mail : %s", found_client.mail);
            printf("\nMot de passe : %s", found_client.pw);
            printf("\nTotal achats : %d\n", found_client.nb_achats);
        }
        else {
            printf("\nClient introuvable !\n");
        }
    }

    go_home();
}

void modifier_client() {
    int toedit_id;
    struct Client c;
    FILE *file, *tempFile;
    bool edited = false;
    char edited_client_name[50];

    printf("\nPour modifier un client, renseignez son ID : ");
    scanf("%d", &toedit_id);

    file = fopen("clients.dat", "rb");
    if(file == NULL) {
        perror("Erreur lors de l'ouverture du fichier.");
        return;
    }

    tempFile = fopen("temp.dat", "wb");
    if(tempFile == NULL) {
        perror("Erreur lors de la création du fichier temp.");
        fclose(file);
        return;
    }

    while(fread(&c, sizeof(struct Client), 1, file)) {
        if(c.id == toedit_id) {
            printf("Modification du client No.%d (%s) :\n", toedit_id, c.nom);
            printf("Nouvel ID : ");
            scanf("%d", &c.id);
            printf("Nom du Client : ");
            cleanBuffer();
            fgets(c.nom, sizeof(c.nom), stdin);
            c.nom[strcspn(c.nom, "\n")] = 0;

            printf("Adresse mail : ");
            fgets(c.mail, sizeof(c.mail), stdin);
            c.mail[strcspn(c.mail, "\n")] = 0;

            printf("Mot de passe : ");
            fgets(c.pw, sizeof(c.pw), stdin);
            c.pw[strcspn(c.pw, "\n")] = 0;

            strcpy(edited_client_name, c.nom);
            edited = true;
        }
        fwrite(&c, sizeof(struct Client), 1, tempFile);
    }

    fclose(file);
    fclose(tempFile);

    remove("clients.dat");
    rename("temp.dat", "clients.dat");

    if(edited == true)
        printf("\nLe client No.%d (%s) a bien ete modifie.\n", toedit_id, edited_client_name);
    else
        printf("\nLe client est introuvable.\n");

    go_home();
}

void historique_achats(int arg_role) { //role == 1 : c'est le gestionnaire, role == 2 : cest client
    int id_vente = 0;
    int vente_i = 0;
    struct Vente found_vente;
    struct Vente tableau_ventes[MAX_VENTES];
    struct Vente v;
    FILE *file, *login_f, *client_file;
    struct Client client, c;
    file = fopen("ventes.dat", "rb");
    client_file = fopen("clients.dat", "rb");
    bool client_found = false;

    if(arg_role == 1) { //gestionnaire
        if(file == NULL) {
            perror("Aucun achat actuellement.");
            fclose(file);
        }
        else {
            if(client_file == NULL) {
                perror("Aucun client trouve avec cet ID.");
                fclose(client_file);
                go_home();
            }
            int id_target;

            printf("\nID du client dont vous voulez l'historique : ");
            scanf("%d", &id_target);

            while(fread(&c, sizeof(struct Client), 1, client_file)) {
                if(c.id == id_target) {
                    client_found = true;
                    client = c;
                }
            }

            if(client_found == true) {
                printf("\nHistorique des achats [Client] - %s:\n", client.nom);
                printf("\n_______________________\n");

                while(fread(&v, sizeof(struct Vente), 1, file) && vente_i < MAX_VENTES) {
                    if(strcmp(v.client_name, client.nom) == 0){
                        char edited_vente_name[100];
                        strcpy(edited_vente_name, v.client_name);
                        printf("%d : %s\nLot de %d achete pour un total de %d $\n\n", v.id, v.product_name, v.quantity, v.amount);
                        vente_i++;
                    }
                }
                fclose(file);
            }
            else {
                printf("Client introuvable\n");
                go_home();
            }
        }
    }
    else if(arg_role == 2) { //client
        login_f = fopen("logged.dat", "rb");
        bool connected = false;

        if(login_f == NULL) {
            perror("Vous devez vous connecter !");
            connexion();
        }
        while(fread(&c, sizeof(struct Client), 1, login_f)) {
            connected = true;
            client = c;
        }
        fclose(login_f);

        if(connected == true) {
            printf("`\nVotre historique d'achats :\n");
            printf("`\n___________________________\n");
            while(fread(&v, sizeof(struct Vente), 1, file)) {
                if(strcmp(v.client_name, client.nom) == 0){
                    printf("%d : %s\nVous avez achete un lot de %d pour un total de %d $\n\n", v.id, v.product_name, v.quantity, v.amount);
                }
            }
        }
    }
    go_home();
}

void modifier_historique_achats(int arg_role) { //1: gestio, 2:client

}

void consulter_fournisseur() { //Pour gestionnaire
    int id_fourni = 0;
    int fourni_i = 0;
    struct Fournisseur found_fourni;
    struct Fournisseur tableau_fournis[MAX_FOURNISSEURS];
    struct Fournisseur f;
    FILE *file;
    file = fopen("fournisseurs.dat", "rb");

    if(file == NULL) {
        perror("Aucun fournisseur actuellement.\n");
    }
    else {
        printf("\nListe des fournisseurs :\n");

        while(fread(&f, sizeof(struct Fournisseur), 1, file) && fourni_i < MAX_FOURNISSEURS) {
            printf("%d : %s\n", f.id, f.name);
            tableau_fournis[fourni_i] = f;
            fourni_i++;
        }
        fclose(file);

        printf("\nRenseignez l'ID du fournisseur a consulter : ");
        scanf("%d", &id_fourni);

        for(int i = 0;i < fourni_i;i++) {
            if(id_fourni == tableau_fournis[i].id) {
                found_fourni = tableau_fournis[i];
            }
        }

        if(id_fourni == found_fourni.id) {
            printf("\n\nID : %d", found_fourni.id);
            printf("\nNom : %s", found_fourni.name);
            printf("\nMot de passe : %s", found_fourni.pw);
            printf("\nNombre de ventes : %d\n", found_fourni.nb_ventes);
        }
        else {
            printf("\nFournisseur introuvable !\n");
        }
    }

    go_home();
}

void modifier_fournisseur() { //Pour gestionnaire
    int toedit_id;
    struct Fournisseur f;
    FILE *file, *tempFile;
    bool edited = false;
    char edited_fourni_name[50];

    printf("\nPour modifier un fournisseur, renseignez son ID : ");
    scanf("%d", &toedit_id);

    file = fopen("fournisseurs.dat", "rb");
    if(file == NULL) {
        perror("Erreur lors de l'ouverture du fichier.");
        return;
    }

    tempFile = fopen("temp.dat", "wb");
    if(tempFile == NULL) {
        perror("Erreur lors de la création du fichier temp.");
        fclose(file);
        return;
    }

    while(fread(&f, sizeof(struct Fournisseur), 1, file)) {
        if(f.id == toedit_id) {
            printf("Modification du fournisseur No.%d (%s) :\n", toedit_id, f.name);
            printf("Nouvel ID : ");
            scanf("%d", &f.id);
            printf("Nom du Fournisseur : ");
            cleanBuffer();
            fgets(f.name, sizeof(f.name), stdin);
            f.name[strcspn(f.name, "\n")] = 0;

            printf("Mot de passe : ");
            fgets(f.pw, sizeof(f.pw), stdin);
            f.pw[strcspn(f.pw, "\n")] = 0;

            strcpy(edited_fourni_name, f.name);
            edited = true;
        }
        fwrite(&f, sizeof(struct Fournisseur), 1, tempFile);
    }

    fclose(file);
    fclose(tempFile);

    remove("fournisseurs.dat");
    rename("temp.dat", "fournisseurs.dat");

    if(edited == true)
        printf("\nLe fournisseur No.%d (%s) a bien ete modifie.\n", toedit_id, edited_fourni_name);
    else
        printf("\nLe fournisseur est introuvable.\n");

    go_home();
}

void ajouter_fournisseur() {
    struct Fournisseur f;
    FILE *file;

    printf("\nPour creer un Fournisseur, renseignez les infos suivantes : ");
    printf("\nID : ");
    scanf("%d", &f.id);
    printf("\nNom d'entreprise : ");
    cleanBuffer();
    fgets(f.name, sizeof(f.name), stdin);
    f.name[strcspn(f.name, "\n")] = 0;
    printf("Son password : ");
    fgets(f.pw, sizeof(f.pw), stdin);
    f.pw[strcspn(f.pw, "\n")] = 0;
    f.nb_ventes = 0;

    file = fopen("fournisseurs.dat", "ab");

    if(file == NULL) {
        perror("Erreur lors de l'ouverture du fichier.");
    }

    fwrite(&f, sizeof(struct Fournisseur), 1, file);
    fclose(file);

    printf("\nLe Fournisseur No.%d [%s] a bien ete cree.\n", f.id, f.name);
    go_home();
}

void fournir_produit() { //Pour fournisseur
    int toedit_id;
    struct Produit p;
    struct Fournisseur f;
    FILE *file, *tempFile, *fourni_file;
    bool edited = false;
    char edited_product_name[50];
    int added_quantity = 0;
    int old_stock;
    int id_login;
    char pw_login[50];
    bool fourni_trouve = false;

    do
    {
        printf("\n\nVotre ID [Fournisseur] : ");
        scanf("%d", &id_login);

        printf("Votre mot de passe [Fournisseur] : ");
        scanf(" %s", &pw_login);
        fourni_file = fopen("fournisseurs.dat", "rb");

        while(fread(&f, sizeof(struct Fournisseur), 1, fourni_file)) {
            if(f.id == id_login && strcmp(f.pw, pw_login) == 0)
                fourni_trouve = true;
        }

    } while (fourni_trouve == false);

    printf("\nConnexion en tant que Fournisseur reussie !\n");

    printf("\nPour ajouter du stock a un produit, renseignez son ID : ");
    scanf("%d", &toedit_id);

    file = fopen("produits.dat", "rb");
    if(file == NULL) {
        perror("Erreur lors de l'ouverture du fichier.");
        return;
    }

    tempFile = fopen("temp.dat", "wb");
    if(tempFile == NULL) {
        perror("Erreur lors de la création du fichier temp.");
        fclose(file);
        return;
    }

    while(fread(&p, sizeof(struct Produit), 1, file)) {
        if(p.id == toedit_id) {
            old_stock = p.avail_stock;
            printf("\nAjout de stock au produit No.%d (%s) :\n", toedit_id, p.name);

            printf("Quantite fournie : ");
            scanf("%d", &added_quantity);
            p.avail_stock += added_quantity;
            strcpy(edited_product_name, p.name);
            edited = true;
        }
        fwrite(&p, sizeof(struct Produit), 1, tempFile);
    }

    fclose(file);
    fclose(tempFile);

    remove("produits.dat");
    rename("temp.dat", "produits.dat");

    if(edited == true) {
        printf("\nLe stock du produit No.%d (%s) a bien ete modifie. (+%d)\n", toedit_id, edited_product_name, added_quantity);
        printf("\nAncien stock : %d         Nouveau stock : %d\n", old_stock, old_stock + added_quantity);
    }
    else
        printf("\nLe produit est introuvable.\n");

    go_home();
}

int choix_action(int choice, int role) {
    //On check le role puis selon ce role on check l'action
    switch (role)
    {
    case 1:
        //Gestionnaire: choice de 1 a 4 possible
        if(choice == 1) {
            int produit_choix = 0;

            while (produit_choix < 1 || produit_choix > 4)
            {
                printf("\n[Gestion Produits] - Selectionnez une action :\n");
                printf("1 : CONSULTER un produit\n");
                printf("2 : AJOUTER un produit\n");
                printf("3 : MODIFIER un produit\n");
                printf("4 : SUPPRIMER un produit\n");
                scanf("%d", &produit_choix);
            }

            switch (produit_choix)
            {
            case 1:
                consulter_produit();
                break;
            case 2:
                ajouter_produit();
                break;
            case 3:
                modifier_produit();
                break;
            case 4:
                supprimer_produit();
                break;
            default:
                break;
            }
        }
        else if(choice == 2) {
            int vente_choix = 0;

            while (vente_choix != 1)
            {
                printf("\n[Gestion Ventes] - Selectionnez une action :\n");
                printf("1 : CONSULTER historique des Ventes\n");
                scanf("%d", &vente_choix);
            }
            historique_ventes();
        }
        else if(choice == 3) {
            int client_choix = 0;

            while (client_choix < 1 || client_choix > 4)
            {
                printf("\n[Gestion Clients] - Selectionnez une action :\n");
                printf("1 : CONSULTER un Client\n");
                printf("2 : MODIFIER un Client\n");
                printf("3 : CONSULTER l'historique d'achats d'un Client\n");
                printf("4 : MODIFIER l'historique d'achats d'un Client\n");
                scanf("%d", &client_choix);
            }

            switch (client_choix)
            {
            case 1:
                consulter_client();
                break;
            case 2:
                modifier_client();
                break;
            case 3:
                historique_achats(1);
                break;
            case 4:
                modifier_historique_achats(1);
                break;
            default:
                break;
            }
        }
        else if(choice == 4) {
            int fourni_choix = 0;

            while (fourni_choix != 1 && fourni_choix != 2 && fourni_choix != 3)
            {
                printf("\n[Gestion Fournisseurs] - Selectionnez une action :\n");
                printf("1 : CONSULTER un Fournisseur\n");
                printf("2 : MODIFIER un Fournisseur\n");
                printf("3 : AJOUTER un Fournisseur\n");
                scanf("%d", &fourni_choix);
            }

            switch (fourni_choix)
            {
            case 1:
                consulter_fournisseur();
                break;
            case 2:
                modifier_fournisseur();
                break;
            case 3:
                ajouter_fournisseur();
                break;
            default:
                break;
            }
        }
        break;
    case 2:
        //Client: choice de 1 a 4 possible
        if(choice == 1) {
            consulter_produit();
        }
        else if(choice == 2) {
            acheter_produit();
        }
        else if(choice == 3) {
            historique_achats(2);
        }
        else if(choice == 4) {
            modifier_historique_achats(2);
        }
        break;
    case 3:
      //  int reset;


        //Fournisseur: choice de 1 a 2 possible
        if(choice == 1) {
            fournir_produit();
        }
        else if(choice == 2) {
            consulter_produit();
        }
        break;
    default:
        break;
    }
}

int accueil_gestionnaire() {
    int choix_menu = 0;

    printf("\n\nVous etes connecte en tant que GESTIONNAIRE.\n");

    while(choix_menu < 1 || choix_menu > 4) {
        printf("Que souhaitez-vous faire ? (1 a 4)\n");
        printf("1 : Gestion Produits\n2 : Gestion Ventes\n3 : Gestion Clients\n4 : Gestion Fournisseurs\n");
        scanf("%d", &choix_menu);
    }

    //On s'assure du chiffre
    if(choix_menu == 1 || choix_menu == 2 || choix_menu == 3 || choix_menu == 4) {
        choix_action(choix_menu, 1); //arg1 : int du choix, arg2: int du role
    }
    else
        return 1;
}

void accueil_client(bool connection_state) {
    bool connecte = connection_state;

    if(connecte) {
        int choix_menu = 0;
        printf("\n\nVous etes connecte en tant que [CLIENT].\n");

        while(choix_menu < 1 || choix_menu > 4) {
            printf("Que souhaitez-vous faire ? (1 a 4)\n");
            printf("1 : Consulter un Produit\n2 : Acheter un Produit\n3 : Voir votre Historique d'achats\n4 : Modifier votre Historique d'achats\n");
            scanf("%d", &choix_menu);
        }

        //On s'assure du chiffre
        if(choix_menu == 1 || choix_menu == 2 || choix_menu == 3 || choix_menu == 4) {
            choix_action(choix_menu, 2); //arg1 : int du choix, arg2: int du role
        }
        else
            return;
    }
    else {
        char login_choice = '0';

        printf("\nVous devez etre connecte pour acceder ici.");
        printf("\n1 : Connexion");
        printf("\n2 : Creer votre compte");
        printf("\nany : Quitter\n");
        scanf(" %c", &login_choice);

        if(login_choice == '1')
            connexion();
        else if(login_choice == '2')
            ajouter_client();
        else
            go_home();
    }
}

int accueil_fournisseur() {
    int choix_menu = 0;

    printf("\n\nVous etes connecte en tant que FOURNISSEUR.\n");

    while(choix_menu < 1 || choix_menu > 2)
    {
        printf("Que souhaitez-vous faire ? (1 a 2)\n");
        printf("1 : Fournir un Produit\n2 : Consulter un Produit\n");
        scanf("%d", &choix_menu);
    }

    //On s'assure du chiffre
    if(choix_menu == 1 || choix_menu == 2) {
        choix_action(choix_menu, 3); //arg1 : int du choix, arg2: int du role
    }
    else
        return 1;
}

void home_all() {
    int role = 0;

    printf("Choisissez votre role svp :\n");
    printf("1 : GESTIONNAIRE\n");
    printf("2 : CLIENT\n");
    printf("3 : FOURNISSEUR\n");
    scanf("%d", &role);

    //on s'assure que le role est bien entre 1 et 3
    while(role < 1 || role > 3)
    {
        printf("Vous devez entrer un chiffre entre 1 et 3 :\n");
        scanf("%d", &role);
    }

    //On gere les 3 possibilites
    switch (role)
    {
    case 1:
        accueil_gestionnaire();
        break;
    case 2:
        accueil_client(false);
        break;
    case 3:
        accueil_fournisseur();
        break;
    default:
        break;
    }
    return;
}

void go_home() {
    char nav_choix;
    printf("\na : Accueil,       any : Fermer programme\n");
    scanf(" %c", &nav_choix);

    if(nav_choix == 'a')
        home_all();
}

int main() {
    printf("Bienvenue !\n");
    home_all();
    printf("\nFermeture... A bientot !\n\n");
    remove("logged.dat");
    return 0;
}

Le mer. 17 avr. 2024 à 22:55, zemni dhia <dhiazemnipf@gmail.com> a écrit :
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define MAX_PRODUITS 255
#define MAX_VENTES 255
#define MAX_FOURNISSEURS 255
#define MAX_CLIENTS 255
void go_home(); //proto de go_home
void accueil_client(bool connection_state);

struct Client {
    int id;
    char nom[50];
    char mail[50];
    char pw[50];
    int nb_achats;
};

struct Fournisseur {
    int id;
    char name[50];
    char pw[50];
    int nb_ventes;
};

struct Produit {
    int id;
    char name[50];
    char desc[50];
    int price;
    int avail_stock;
    int nb_ventes;
};

struct Vente {
    int id;
    char product_name[50];
    int quantity;
    int amount;
    char client_name[50];
};


void cleanBuffer() { // Fonction pour nettoyer le tampon d'entrée (a cause de fgets/scanf bug)
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }

    return;
}

void ajouter_produit() {
    struct Produit p;
    FILE *file;

    printf("\nPour creer un produit, veuillez renseigner certaines infos :");
    printf("\nID : ");
    scanf("%d", &p.id);
    printf("Titre : ");
    cleanBuffer();
    fgets(p.name, sizeof(p.name), stdin);
    p.name[strcspn(p.name, "\n")] = 0; // Supprime le retour à la ligne
    printf("Description : ");
    fgets(p.desc, sizeof(p.desc), stdin);
    p.desc[strcspn(p.desc, "\n")] = 0; // Supprime le retour à la ligne
    printf("Prix : ");
    scanf("%d", &p.price);
    printf("Stock disponible : ");
    scanf("%d", &p.avail_stock);
    p.nb_ventes = 0;

    file = fopen("produits.dat", "ab"); //append binary pour créer auto

    if(file == NULL) {
        perror("Erreur lors de l'ouverture du fichier.");
    }

    fwrite(&p, sizeof(struct Produit), 1, file);
    fclose(file);

    printf("\nLe produit a bien ete cree.\n");
    go_home();
}

void modifier_produit() {
    int toedit_id;
    struct Produit p;
    FILE *file, *tempFile;
    bool edited = false;
    char edited_product_name[50];

    printf("\nPour modifier un produit, renseignez son ID : ");
    scanf("%d", &toedit_id);

    file = fopen("produits.dat", "rb");
    if(file == NULL) {
        perror("Erreur lors de l'ouverture du fichier.");
        return;
    }

    tempFile = fopen("temp.dat", "wb");
    if(tempFile == NULL) {
        perror("Erreur lors de la création du fichier temp.");
        fclose(file);
        return;
    }

    while(fread(&p, sizeof(struct Produit), 1, file)) {
        if(p.id == toedit_id) {
            printf("Modification du produit No.%d (%s) :\n", toedit_id, p.name);

            printf("Nom du Produit : ");
            cleanBuffer();
            fgets(p.name, sizeof(p.name), stdin);
            p.name[strcspn(p.name, "\n")] = 0; // Supprime le retour à la ligne

            printf("Description : ");
            fgets(p.desc, sizeof(p.desc), stdin);
            p.desc[strcspn(p.desc, "\n")] = 0; // Supprime le retour à la ligne

            printf("Prix : ");
            scanf("%d", &p.price);
            printf("Stock dispo : ");
            scanf("%d", &p.avail_stock);

            strcpy(edited_product_name, p.name);
            edited = true;
        }
        fwrite(&p, sizeof(struct Produit), 1, tempFile);
    }

    fclose(file);
    fclose(tempFile);

    remove("produits.dat");
    rename("temp.dat", "produits.dat");

    if(edited == true)
        printf("\nLe produit No.%d (%s) a bien ete modifie.\n", toedit_id, edited_product_name);
    else
        printf("\nLe produit est introuvable.\n");

    go_home();
}

void supprimer_produit() {
    int todel_id;
    struct Produit p;
    FILE *file, *tempFile;
    bool deleted = false;
    char deleted_product_name[50];

    printf("\nPour supprimer un produit, renseignez son ID : ");
    scanf("%d", &todel_id);

    file = fopen("produits.dat", "rb");
    if(file == NULL) {
        perror("Erreur lors de l'ouverture du fichier.");
        return;
    }

    tempFile = fopen("temp.dat", "wb");
    if(tempFile == NULL) {
        perror("Erreur lors de la création du fichier temp.");
        fclose(file);
        return;
    }

    while(fread(&p, sizeof(struct Produit), 1, file)) {
        if(p.id != todel_id) {
            fwrite(&p, sizeof(struct Produit), 1, tempFile);
        }
        else {
            strcpy(deleted_product_name, p.name);
            deleted = true;
        }
    }

    fclose(file);
    fclose(tempFile);

    remove("produits.dat");
    rename("temp.dat", "produits.dat");

    if(deleted == true)
        printf("\nLe produit %d (%s) a bien ete supprime.\n", todel_id, deleted_product_name);
    else
        printf("\nLe produit est introuvable.\n");

    go_home();
}

void consulter_produit() {
    int id_produit = 0;
    int produit_i = 0;
    struct Produit found_produit;
    struct Produit tableau_produits[MAX_PRODUITS];
    struct Produit p;
    FILE *file;
    file = fopen("produits.dat", "rb");

    if(file == NULL) {
        perror("Aucun produit actuellement.");
    }
    else {
        printf("\nListe des produits :\n");

        while(fread(&p, sizeof(struct Produit), 1, file) && produit_i < MAX_PRODUITS) {
            printf("%d : %s\n", p.id, p.name);
            tableau_produits[produit_i] = p;
            produit_i++;
        }
        fclose(file);

        printf("\nRenseignez l'id du produit a consulter : ");
        scanf("%d", &id_produit);

        for(int i = 0;i < produit_i;i++) {
            if(id_produit == tableau_produits[i].id) {
                found_produit = tableau_produits[i];
            }
        }

        if(id_produit == found_produit.id) {
            printf("\n\nID : %d", found_produit.id);
            printf("\nProduit : %s", found_produit.name);
            printf("\nDescription : %s", found_produit.desc);
            printf("\nPrix : %d $", found_produit.price);
            printf("\nStock dispo : %d", found_produit.avail_stock);
            printf("\nNb Ventes : %d\n", found_produit.nb_ventes);
        }
        else {
            printf("\nProduit introuvable !\n");
        }
    }

    go_home();
}

void acheter_produit() {
    int produit_id;
    FILE *connected_usr = fopen("logged.dat", "rb");
    FILE *fichier_produits = fopen("produits.dat", "rb+");
    FILE *fichier_ventes = fopen("ventes.dat", "ab");
    FILE *fichier_clients = fopen("clients.dat", "ab");
    struct Produit p;
    struct Client c;
    bool produit_found = false;
    int quantity = 1; //quantité a 1 de base
    char client_name[50];

    if (connected_usr == NULL) {
        perror("Vous devez etre connecte");
        go_home();
    }
    else {
        while(fread(&c, sizeof(struct Client), 1, connected_usr)) {
            strcpy(client_name, c.nom);
        }
    }

    printf("Entrez l'ID du produit que vous souhaitez acheter : ");
    scanf("%d", &produit_id);
    printf("Quantite : ");
    scanf("%d", &quantity);

    if (fichier_produits == NULL || fichier_ventes == NULL) {
        perror("Erreur lors de l'ouverture d'un fichier");
        return;
    }

    //On actualise produits
    while (fread(&p, sizeof(struct Produit), 1, fichier_produits)) {
        if (p.id == produit_id && p.avail_stock >= quantity) {
            produit_found = true;
            p.avail_stock -= quantity; // Reduction du stock
            p.nb_ventes += quantity; // Incrémentation du nombre de ventes
            fseek(fichier_produits, -(long)sizeof(struct Produit), SEEK_CUR); // Retour a la position du produit lu
            fwrite(&p, sizeof(struct Produit), 1, fichier_produits); // Mise a jour du produit dans le fichier
            break;
        }
    }

    if (produit_found) {
        struct Vente nouvelleVente = {
            .id = produit_id, // Utiliser un ID unique pour chaque vente pourrait être une meilleure approche
            .quantity = quantity,
            .amount = p.price * quantity
        };
        strcpy(nouvelleVente.product_name, p.name);
        strcpy(nouvelleVente.client_name, client_name);

        fwrite(&nouvelleVente, sizeof(struct Vente), 1, fichier_ventes);

        //On actualise clients
        while (fread(&c, sizeof(struct Client), 1, fichier_clients)) {
            if (c.nom == client_name) {
                c.nb_achats += quantity; // Incrémentation du nombre dachats
                fseek(fichier_clients, -(long)sizeof(struct Client), SEEK_CUR); // Retour a la position du produit lu
                fwrite(&c, sizeof(struct Client), 1, fichier_clients); // Mise a jour du client avec le nouveau nb_achats
                break;
            }
        }

        printf("\n\nProduit achete avec succes !\n");
        printf("ID : %d\n", p.id);
        printf("Nom : %s\n", p.name);
        printf("Prix : %d $\n", p.price);
        printf("Quantite : %d\n", quantity);
        printf("Total paye : %d $\n", quantity * p.price);
    } else {
        printf("Produit non trouve ou stock insuffisant.\n");
    }

    fclose(fichier_produits);
    fclose(fichier_ventes);
    go_home();
}

void historique_ventes() {
    int id_vente = 0;
    int vente_i = 0;
    struct Vente found_vente;
    struct Vente tableau_ventes[MAX_VENTES];
    struct Vente v;
    FILE *file;
    file = fopen("ventes.dat", "rb");

    if(file == NULL) {
        perror("Aucune vente actuellement.");
    }
    else {
        printf("\nHistorique des ventes :");
        printf("\n_______________________\n");

        while(fread(&v, sizeof(struct Vente), 1, file) && vente_i < MAX_VENTES) {
            char edited_vente_name[100];
            strcpy(edited_vente_name, v.client_name);
            printf("%d : %s\nLot de %d achete par %s pour un total de %d $\n\n", v.id, v.product_name, v.quantity, edited_vente_name, v.amount);
            tableau_ventes[vente_i] = v;
            vente_i++;
        }
        fclose(file);
    }

    go_home();
}

int check_connected(char *mail, char *pw) { //retourne 0 si connected sinon 1
    FILE *file; //Les clients incsrits
    FILE *file_connected; //Le client connecte
    struct Client c;
    int trouve = 0;
    int connected = 0;

    file = fopen("clients.dat", "rb");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier.");
        return 1;
    }

    while(fread(&c, sizeof(struct Client), 1, file)) {
        if(strcmp(c.mail, mail) == 0 && strcmp(c.pw, pw) == 0) {
            trouve = 1;

            file_connected = fopen("logged.dat", "ab");
            if(file_connected == NULL) {
                fclose(file_connected);
            }
            fwrite(&c, sizeof(struct Client), 1, file_connected);
            fclose(file_connected);
            connected = 1;
            break;
        }
    }
    fclose(file);

    if(trouve && connected) {
        return 0;
    }
    else {
        return 1;
    }
}

void connexion() { //Menu du choix Connexion/Inscription
    printf("Pour vous connecter, veuillez saisir ces infos :\n");
    char login_mail[100];
    char login_pw[100];

    printf("\nEmail : ");
    scanf("%s", &login_mail);
    printf("Mot de passe : ");
    scanf("%s", &login_pw);

    if(check_connected(login_mail, login_pw) == 0) {
        accueil_client(true);
    } else {
        int retry_choice = 0;

        printf("\nEmail ou mot de passe incorrect.\n");
        printf("\na : Reessayer,     any : Quitter.\n");
        scanf(" %c", &retry_choice);

        if(retry_choice == 'a')
            connexion();
        else
            go_home();
    }
}

void ajouter_client() {
    struct Client c;
    FILE *file;

    printf("\nPour creer un compte, renseignez les infos suivantes : ");
    printf("\nID : ");
    scanf("%d", &c.id);
    printf("\nVotre nom complet : ");
    cleanBuffer();
    fgets(c.nom, sizeof(c.nom), stdin);
    c.nom[strcspn(c.nom, "\n")] = 0; // Supprime le retour à la ligne
    printf("Votre e-mail : ");
    fgets(c.mail, sizeof(c.mail), stdin);
    c.mail[strcspn(c.mail, "\n")] = 0; // Supprime le retour à la ligne
    printf("Votre password : ");
    fgets(c.pw, sizeof(c.pw), stdin);
    c.pw[strcspn(c.pw, "\n")] = 0;
    c.nb_achats = 0;

    file = fopen("clients.dat", "ab"); //append binary pour créer auto

    if(file == NULL) {
        perror("Erreur lors de l'ouverture du fichier.");
    }

    fwrite(&c, sizeof(struct Client), 1, file);
    fclose(file);

    printf("\nVotre profil a bien ete cree.\n");
    connexion();
}

void consulter_client() {
    int id_client = 0;
    int client_i = 0;
    struct Client found_client;
    struct Client tableau_clients[MAX_CLIENTS];
    struct Client c;
    FILE *file;
    file = fopen("clients.dat", "rb");

    if(file == NULL) {
        perror("Aucun client actuellement.\n");
    }
    else {
        printf("\nListe des clients :\n");

        while(fread(&c, sizeof(struct Client), 1, file) && client_i < MAX_CLIENTS) {
            printf("%d : %s\n", c.id, c.nom);
            tableau_clients[client_i] = c;
            client_i++;
        }
        fclose(file);

        printf("\nRenseignez l'ID du client a consulter : ");
        scanf("%d", &id_client);

        for(int i = 0;i < client_i;i++) {
            if(id_client== tableau_clients[i].id) {
                found_client = tableau_clients[i];
            }
        }

        if(id_client == found_client.id) {
            printf("\n\nID : %d", found_client.id);
            printf("\nNom : %s", found_client.nom);
            printf("\nAdresse Mail : %s", found_client.mail);
            printf("\nMot de passe : %s", found_client.pw);
            printf("\nTotal achats : %d\n", found_client.nb_achats);
        }
        else {
            printf("\nClient introuvable !\n");
        }
    }

    go_home();
}

void modifier_client() {
    int toedit_id;
    struct Client c;
    FILE *file, *tempFile;
    bool edited = false;
    char edited_client_name[50];

    printf("\nPour modifier un client, renseignez son ID : ");
    scanf("%d", &toedit_id);

    file = fopen("clients.dat", "rb");
    if(file == NULL) {
        perror("Erreur lors de l'ouverture du fichier.");
        return;
    }

    tempFile = fopen("temp.dat", "wb");
    if(tempFile == NULL) {
        perror("Erreur lors de la création du fichier temp.");
        fclose(file);
        return;
    }

    while(fread(&c, sizeof(struct Client), 1, file)) {
        if(c.id == toedit_id) {
            printf("Modification du client No.%d (%s) :\n", toedit_id, c.nom);
            printf("Nouvel ID : ");
            scanf("%d", &c.id);
            printf("Nom du Client : ");
            cleanBuffer();
            fgets(c.nom, sizeof(c.nom), stdin);
            c.nom[strcspn(c.nom, "\n")] = 0;

            printf("Adresse mail : ");
            fgets(c.mail, sizeof(c.mail), stdin);
            c.mail[strcspn(c.mail, "\n")] = 0;

            printf("Mot de passe : ");
            fgets(c.pw, sizeof(c.pw), stdin);
            c.pw[strcspn(c.pw, "\n")] = 0;

            strcpy(edited_client_name, c.nom);
            edited = true;
        }
        fwrite(&c, sizeof(struct Client), 1, tempFile);
    }

    fclose(file);
    fclose(tempFile);

    remove("clients.dat");
    rename("temp.dat", "clients.dat");

    if(edited == true)
        printf("\nLe client No.%d (%s) a bien ete modifie.\n", toedit_id, edited_client_name);
    else
        printf("\nLe client est introuvable.\n");

    go_home();
}

void historique_achats(int arg_role) { //role == 1 : c'est le gestionnaire, role == 2 : cest client
    int id_vente = 0;
    int vente_i = 0;
    struct Vente found_vente;
    struct Vente tableau_ventes[MAX_VENTES];
    struct Vente v;
    FILE *file, *login_f, *client_file;
    struct Client client, c;
    file = fopen("ventes.dat", "rb");
    client_file = fopen("clients.dat", "rb");
    bool client_found = false;

    if(arg_role == 1) { //gestionnaire
        if(file == NULL) {
            perror("Aucun achat actuellement.");
            fclose(file);
        }
        else {
            if(client_file == NULL) {
                perror("Aucun client trouve avec cet ID.");
                fclose(client_file);
                go_home();
            }
            int id_target;

            printf("\nID du client dont vous voulez l'historique : ");
            scanf("%d", &id_target);

            while(fread(&c, sizeof(struct Client), 1, client_file)) {
                if(c.id == id_target) {
                    client_found = true;
                    client = c;
                }
            }

            if(client_found == true) {
                printf("\nHistorique des achats [Client] - %s:\n", client.nom);
                printf("\n_______________________\n");

                while(fread(&v, sizeof(struct Vente), 1, file) && vente_i < MAX_VENTES) {
                    if(strcmp(v.client_name, client.nom) == 0){
                        char edited_vente_name[100];
                        strcpy(edited_vente_name, v.client_name);
                        printf("%d : %s\nLot de %d achete pour un total de %d $\n\n", v.id, v.product_name, v.quantity, v.amount);
                        vente_i++;
                    }
                }
                fclose(file);
            }
            else {
                printf("Client introuvable\n");
                go_home();
            }
        }
    }
    else if(arg_role == 2) { //client
        login_f = fopen("logged.dat", "rb");
        bool connected = false;

        if(login_f == NULL) {
            perror("Vous devez vous connecter !");
            connexion();
        }
        while(fread(&c, sizeof(struct Client), 1, login_f)) {
            connected = true;
            client = c;
        }
        fclose(login_f);

        if(connected == true) {
            printf("`\nVotre historique d'achats :\n");
            printf("`\n___________________________\n");
            while(fread(&v, sizeof(struct Vente), 1, file)) {
                if(strcmp(v.client_name, client.nom) == 0){
                    printf("%d : %s\nVous avez achete un lot de %d pour un total de %d $\n\n", v.id, v.product_name, v.quantity, v.amount);
                }
            }
        }
    }
    go_home();
}

void modifier_historique_achats(int arg_role) { //1: gestio, 2:client

}

void consulter_fournisseur() { //Pour gestionnaire
    int id_fourni = 0;
    int fourni_i = 0;
    struct Fournisseur found_fourni;
    struct Fournisseur tableau_fournis[MAX_FOURNISSEURS];
    struct Fournisseur f;
    FILE *file;
    file = fopen("fournisseurs.dat", "rb");

    if(file == NULL) {
        perror("Aucun fournisseur actuellement.\n");
    }
    else {
        printf("\nListe des fournisseurs :\n");

        while(fread(&f, sizeof(struct Fournisseur), 1, file) && fourni_i < MAX_FOURNISSEURS) {
            printf("%d : %s\n", f.id, f.name);
            tableau_fournis[fourni_i] = f;
            fourni_i++;
        }
        fclose(file);

        printf("\nRenseignez l'ID du fournisseur a consulter : ");
        scanf("%d", &id_fourni);

        for(int i = 0;i < fourni_i;i++) {
            if(id_fourni == tableau_fournis[i].id) {
                found_fourni = tableau_fournis[i];
            }
        }

        if(id_fourni == found_fourni.id) {
            printf("\n\nID : %d", found_fourni.id);
            printf("\nNom : %s", found_fourni.name);
            printf("\nMot de passe : %s", found_fourni.pw);
            printf("\nNombre de ventes : %d\n", found_fourni.nb_ventes);
        }
        else {
            printf("\nFournisseur introuvable !\n");
        }
    }

    go_home();
}

void modifier_fournisseur() { //Pour gestionnaire
    int toedit_id;
    struct Fournisseur f;
    FILE *file, *tempFile;
    bool edited = false;
    char edited_fourni_name[50];

    printf("\nPour modifier un fournisseur, renseignez son ID : ");
    scanf("%d", &toedit_id);

    file = fopen("fournisseurs.dat", "rb");
    if(file == NULL) {
        perror("Erreur lors de l'ouverture du fichier.");
        return;
    }

    tempFile = fopen("temp.dat", "wb");
    if(tempFile == NULL) {
        perror("Erreur lors de la création du fichier temp.");
        fclose(file);
        return;
    }

    while(fread(&f, sizeof(struct Fournisseur), 1, file)) {
        if(f.id == toedit_id) {
            printf("Modification du fournisseur No.%d (%s) :\n", toedit_id, f.name);
            printf("Nouvel ID : ");
            scanf("%d", &f.id);
            printf("Nom du Fournisseur : ");
            cleanBuffer();
            fgets(f.name, sizeof(f.name), stdin);
            f.name[strcspn(f.name, "\n")] = 0;

            printf("Mot de passe : ");
            fgets(f.pw, sizeof(f.pw), stdin);
            f.pw[strcspn(f.pw, "\n")] = 0;

            strcpy(edited_fourni_name, f.name);
            edited = true;
        }
        fwrite(&f, sizeof(struct Fournisseur), 1, tempFile);
    }

    fclose(file);
    fclose(tempFile);

    remove("fournisseurs.dat");
    rename("temp.dat", "fournisseurs.dat");

    if(edited == true)
        printf("\nLe fournisseur No.%d (%s) a bien ete modifie.\n", toedit_id, edited_fourni_name);
    else
        printf("\nLe fournisseur est introuvable.\n");

    go_home();
}

void ajouter_fournisseur() {
    struct Fournisseur f;
    FILE *file;

    printf("\nPour creer un Fournisseur, renseignez les infos suivantes : ");
    printf("\nID : ");
    scanf("%d", &f.id);
    printf("\nNom d'entreprise : ");
    cleanBuffer();
    fgets(f.name, sizeof(f.name), stdin);
    f.name[strcspn(f.name, "\n")] = 0;
    printf("Son password : ");
    fgets(f.pw, sizeof(f.pw), stdin);
    f.pw[strcspn(f.pw, "\n")] = 0;
    f.nb_ventes = 0;

    file = fopen("fournisseurs.dat", "ab");

    if(file == NULL) {
        perror("Erreur lors de l'ouverture du fichier.");
    }

    fwrite(&f, sizeof(struct Fournisseur), 1, file);
    fclose(file);

    printf("\nLe Fournisseur No.%d [%s] a bien ete cree.\n", f.id, f.name);
    go_home();
}

void fournir_produit() { //Pour fournisseur
    int toedit_id;
    struct Produit p;
    struct Fournisseur f;
    FILE *file, *tempFile, *fourni_file;
    bool edited = false;
    char edited_product_name[50];
    int added_quantity = 0;
    int old_stock;
    int id_login;
    char pw_login[50];
    bool fourni_trouve = false;

    do
    {
        printf("\n\nVotre ID [Fournisseur] : ");
        scanf("%d", &id_login);

        printf("Votre mot de passe [Fournisseur] : ");
        scanf(" %s", &pw_login);
        fourni_file = fopen("fournisseurs.dat", "rb");

        while(fread(&f, sizeof(struct Fournisseur), 1, fourni_file)) {
            if(f.id == id_login && strcmp(f.pw, pw_login) == 0)
                fourni_trouve = true;
        }

    } while (fourni_trouve == false);

    printf("\nConnexion en tant que Fournisseur reussie !\n");

    printf("\nPour ajouter du stock a un produit, renseignez son ID : ");
    scanf("%d", &toedit_id);

    file = fopen("produits.dat", "rb");
    if(file == NULL) {
        perror("Erreur lors de l'ouverture du fichier.");
        return;
    }

    tempFile = fopen("temp.dat", "wb");
    if(tempFile == NULL) {
        perror("Erreur lors de la création du fichier temp.");
        fclose(file);
        return;
    }

    while(fread(&p, sizeof(struct Produit), 1, file)) {
        if(p.id == toedit_id) {
            old_stock = p.avail_stock;
            printf("\nAjout de stock au produit No.%d (%s) :\n", toedit_id, p.name);

            printf("Quantite fournie : ");
            scanf("%d", &added_quantity);
            p.avail_stock += added_quantity;
            strcpy(edited_product_name, p.name);
            edited = true;
        }
        fwrite(&p, sizeof(struct Produit), 1, tempFile);
    }

    fclose(file);
    fclose(tempFile);

    remove("produits.dat");
    rename("temp.dat", "produits.dat");

    if(edited == true) {
        printf("\nLe stock du produit No.%d (%s) a bien ete modifie. (+%d)\n", toedit_id, edited_product_name, added_quantity);
        printf("\nAncien stock : %d         Nouveau stock : %d\n", old_stock, old_stock + added_quantity);
    }
    else
        printf("\nLe produit est introuvable.\n");

    go_home();
}

int choix_action(int choice, int role) {
    //On check le role puis selon ce role on check l'action
    switch (role)
    {
    case 1:
        //Gestionnaire: choice de 1 a 4 possible
        if(choice == 1) {
            int produit_choix = 0;

            while (produit_choix < 1 || produit_choix > 4)
            {
                printf("\n[Gestion Produits] - Selectionnez une action :\n");
                printf("1 : CONSULTER un produit\n");
                printf("2 : AJOUTER un produit\n");
                printf("3 : MODIFIER un produit\n");
                printf("4 : SUPPRIMER un produit\n");
                scanf("%d", &produit_choix);
            }

            switch (produit_choix)
            {
            case 1:
                consulter_produit();
                break;
            case 2:
                ajouter_produit();
                break;
            case 3:
                modifier_produit();
                break;
            case 4:
                supprimer_produit();
                break;
            default:
                break;
            }
        }
        else if(choice == 2) {
            int vente_choix = 0;

            while (vente_choix != 1)
            {
                printf("\n[Gestion Ventes] - Selectionnez une action :\n");
                printf("1 : CONSULTER historique des Ventes\n");
                scanf("%d", &vente_choix);
            }
            historique_ventes();
        }
        else if(choice == 3) {
            int client_choix = 0;

            while (client_choix < 1 || client_choix > 4)
            {
                printf("\n[Gestion Clients] - Selectionnez une action :\n");
                printf("1 : CONSULTER un Client\n");
                printf("2 : MODIFIER un Client\n");
                printf("3 : CONSULTER l'historique d'achats d'un Client\n");
                printf("4 : MODIFIER l'historique d'achats d'un Client\n");
                scanf("%d", &client_choix);
            }

            switch (client_choix)
            {
            case 1:
                consulter_client();
                break;
            case 2:
                modifier_client();
                break;
            case 3:
                historique_achats(1);
                break;
            case 4:
                modifier_historique_achats(1);
                break;
            default:
                break;
            }
        }
        else if(choice == 4) {
            int fourni_choix = 0;

            while (fourni_choix != 1 && fourni_choix != 2 && fourni_choix != 3)
            {
                printf("\n[Gestion Fournisseurs] - Selectionnez une action :\n");
                printf("1 : CONSULTER un Fournisseur\n");
                printf("2 : MODIFIER un Fournisseur\n");
                printf("3 : AJOUTER un Fournisseur\n");
                scanf("%d", &fourni_choix);
            }

            switch (fourni_choix)
            {
            case 1:
                consulter_fournisseur();
                break;
            case 2:
                modifier_fournisseur();
                break;
            case 3:
                ajouter_fournisseur();
                break;
            default:
                break;
            }
        }
        break;
    case 2:
        //Client: choice de 1 a 4 possible
        if(choice == 1) {
            consulter_produit();
        }
        else if(choice == 2) {
            acheter_produit();
        }
        else if(choice == 3) {
            historique_achats(2);
        }
        else if(choice == 4) {
            modifier_historique_achats(2);
        }
        break;
    case 3:
        int reset;
        //Fournisseur: choice de 1 a 2 possible
        if(choice == 1) {
            fournir_produit();
        }
        else if(choice == 2) {
            consulter_produit();
        }
        break;
    default:
        break;
    }
}

int accueil_gestionnaire() {
    int choix_menu = 0;

    printf("\n\nVous etes connecte en tant que GESTIONNAIRE.\n");

    while(choix_menu < 1 || choix_menu > 4) {
        printf("Que souhaitez-vous faire ? (1 a 4)\n");
        printf("1 : Gestion Produits\n2 : Gestion Ventes\n3 : Gestion Clients\n4 : Gestion Fournisseurs\n");
        scanf("%d", &choix_menu);
    }

    //On s'assure du chiffre
    if(choix_menu == 1 || choix_menu == 2 || choix_menu == 3 || choix_menu == 4) {
        choix_action(choix_menu, 1); //arg1 : int du choix, arg2: int du role
    }
    else
        return 1;
}

void accueil_client(bool connection_state) {
    bool connecte = connection_state;

    if(connecte) {
        int choix_menu = 0;
        printf("\n\nVous etes connecte en tant que [CLIENT].\n");

        while(choix_menu < 1 || choix_menu > 4) {
            printf("Que souhaitez-vous faire ? (1 a 4)\n");
            printf("1 : Consulter un Produit\n2 : Acheter un Produit\n3 : Voir votre Historique d'achats\n4 : Modifier votre Historique d'achats\n");
            scanf("%d", &choix_menu);
        }

        //On s'assure du chiffre
        if(choix_menu == 1 || choix_menu == 2 || choix_menu == 3 || choix_menu == 4) {
            choix_action(choix_menu, 2); //arg1 : int du choix, arg2: int du role
        }
        else
            return;
    }
    else {
        char login_choice = '0';

        printf("\nVous devez etre connecte pour acceder ici.");
        printf("\n1 : Connexion");
        printf("\n2 : Creer votre compte");
        printf("\nany : Quitter\n");
        scanf(" %c", &login_choice);

        if(login_choice == '1')
            connexion();
        else if(login_choice == '2')
            ajouter_client();
        else
            go_home();
    }
}

int accueil_fournisseur() {
    int choix_menu = 0;

    printf("\n\nVous etes connecte en tant que FOURNISSEUR.\n");

    while(choix_menu < 1 || choix_menu > 2)
    {
        printf("Que souhaitez-vous faire ? (1 a 2)\n");
        printf("1 : Fournir un Produit\n2 : Consulter un Produit\n");
        scanf("%d", &choix_menu);
    }

    //On s'assure du chiffre
    if(choix_menu == 1 || choix_menu == 2) {
        choix_action(choix_menu, 3); //arg1 : int du choix, arg2: int du role
    }
    else
        return 1;
}

void home_all() {
    int role = 0;

    printf("Choisissez votre role svp :\n");
    printf("1 : GESTIONNAIRE\n");
    printf("2 : CLIENT\n");
    printf("3 : FOURNISSEUR\n");
    scanf("%d", &role);

    //on s'assure que le role est bien entre 1 et 3
    while(role < 1 || role > 3)
    {
        printf("Vous devez entrer un chiffre entre 1 et 3 :\n");
        scanf("%d", &role);
    }

    //On gere les 3 possibilites
    switch (role)
    {
    case 1:
        accueil_gestionnaire();
        break;
    case 2:
        accueil_client(false);
        break;
    case 3:
        accueil_fournisseur();
        break;
    default:
        break;
    }
    return;
}

void go_home() {
    char nav_choix;
    printf("\na : Accueil,       any : Fermer programme\n");
    scanf(" %c", &nav_choix);

    if(nav_choix == 'a')
        home_all();
}

int main() {
    printf("Bienvenue !\n");
    home_all();
    printf("\nFermeture... A bientot !\n\n");
    remove("logged.dat");
    return 0;
}
