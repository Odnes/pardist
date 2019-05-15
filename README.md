#Dining philosophers: 
	-Resource hierarchy solution προϋποθέτει από πριν γνώση του αριθμού των φιλοσόφων. <-- Ugly
	-Chandy/Misra είναι η βέλτιστη, αλλά παραβιάζει τον κανόνα των σιωπηλών φιλοσόφων (message passing) <--Out of scope, but more appropriate
	-Arbitrator <-- Selected. Seems trivial.

S[N] represents chopstick pairs

#Readers-Writers:
	-Ένα thread reader, ή ένα thread writer, πρέπει να αποφασίσει σε ποιον μεταξύ ενός reader και ενός writer που περιμένουν, θα δώσει σειρά.
	-Η δίκαιη λύση είναι ένας μέγιστος αριθμό threads του ίδιου τύπου που επιτρέπεται να πάρουν προτεραιότητα, εφόσον περιμένει ένα thread του αντίθετου τύπου. Εφόσον δεν υπάρχει τέτοιο thread, ο αριθμός μηδενίζεται και ξεκινάει από την αρχή. FIFO mandatory.


#Producer-Consumer - Sleeping barber:

DONE-Make customers serially infinite, reparallelize. 
DONE-Define number of customer_threads as load factor, passed as argument in main.
DONE-Remove barber's divine status

-Make randwait save wait duration to a global "work_time" counter (gotta be a critical section).
	When work_time reaches shift_duration:
	-If waiting_room empty: Lock waiting room, finish cutting last client's hair, close up shop.
	-If waiting_room != empty: lock waiting room, print id "gets to pay overtime" after each customer's 		
	 hair is cut, close up shop

-Implement bounded queue to prevent resource starvation, add consumer-producer dimension to the problem.
-Implement multiple barberships.