# KHORD_v.1.0.0
KHORD è una tastiera stenografica digitale open-source, a basso costo e personalizzabile, pensata per studenti, scrittori, programmatori e hobbisti, oltre che per professionisti.

Progettata da zero, include un hardware ottimizzato per la stenografia a due mani e un firmware in C per il microcontrollore ESP32-S3, il quale non è supportato dai firmware open-source esistenti (es. QMK).

La repository è organizzata in 4 cartelle:  
- `Hardware/`: schema elettrico, layout PCB, modello 3D del case, file Gerber  
- `Software/`: firmware in C (ESP-IDF) per gestione matrice, debouncing, protocollo GeminiPR, comunicazione USB con Plover
- `Case/`: file per la stampa 3D del case 
- `Documentazione/`: presentazione di KHORD + relazione tecnica con descrizione dettagliata dell'intero progetto e appendici sull'organizzazione dei file hardware e software, sui i metodi principali contenuti e sulle loro funzioni. 
