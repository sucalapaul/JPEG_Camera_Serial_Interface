%Programul principal
%%%%%%%%

program:-
	process('preferinte.txt',Obj),
	process('obiecte.txt',X),
	process('contor.txt',Cont),
	%print_list(Obj),
	getElemente(Obj,O,C),
	(O == 0, getObiecteCuloare(C,X,R1),! ;
	C == 0,getObiecte(O,X,R1),! ;
	getObiecteOC(O,C,X,R1)),
	goObiect(R1,R),
	getFirst(Cont,Cout),
	(not(vida(R)),
		write_list_to_file('comanda.txt',R),
		write_list_to_file('contor.txt',[0]) ;
	vida(R),(Cout < 7,Cout1 is Cout+1,
		write_list_to_file('comanda.txt',R),
		write_list_to_file('contor.txt',[Cout1]))).


%%%%%%%

print_list([]).
print_list([H|T]):-
	write(H),
	print_list(T).

%%%%%%%
%Citeste din fisier
%%%%%

process(File,X) :-
        open(File, read, In),
        read(In, Char1),
        process_stream(Char1, In, X),
        close(In).
 
process_stream(end_of_file, _, []) :- !.
process_stream(Char, In, [Char|X]) :-
        read(In, Char2),
        process_stream(Char2, In, X).

%List vida
vida([]).

%Scrie in fisier
%%%%%%
loop_through_list(F,[N|[]]) :-
    write(F,N),
    write(F,'.').

loop_through_list(F,[]) :-
    write(F,'0'),
    write(F,' '),
    write(F,'5').

loop_through_list(F,(_,_,X,Y)) :-
    write(F,X),
    write(F,' '),
    write(F,Y). 

write_list_to_file(Filename,List) :-
    open(Filename, write, F),
    loop_through_list(F,List),
    close(F).


%Ia toate obiectele care au o anumita culoare
%%%%%%%%%%

getObiecteCuloare(_,[],[]).
getObiecteCuloare(C,[X|T],[X|R]):-
	isCuloare(X,C),!,
	getObiecteCuloare(C,T,R).
getObiecteCuloare(C,[X|T],R):-
	getObiecteCuloare(C,T,R).


%Ia anumite obiecte
%%%%%%%
getObiecte(_,[],[]).
getObiecte(O,[X|T],[X|R]):-
	isObject(X,O),!,
	getObiecte(O,T,R).
getObiecte(O,[X|T],R):-
	getObiecte(O,T,R).


%Ia anumite obiecte de anumite culori
%%%%%%%%
getObiecteOC(_,_,[],[]).
getObiecteOC(O,C,[X|T],[X|R]):-
	isObject(X,O),
	isCuloare(X,C),!,
	getObiecteOC(O,C,T,R).
getObiecteOC(O,C,[X|T],R):-
	getObiecteOC(O,C,T,R).

	
%Verificam culoarea
%%%%%%%%
isCuloare((_,Culoare,_,_),Culoare).


%Verificam obiectul
%%%%%%
isObject((Obiect,_,_,_),Obiect).


%Cautam cel mai apropiat obiect
%%%%%%%%

goObiect([],R,R,_,_).
goObiect([(O,C,X,Y)|T],R,Rez,XX,YY):-
	X=<XX,Y=<YY,!,
	goObiect(T,(O,C,X,Y),Rez,X,Y).
goObiect([_|T],R,Rez,X,Y):-
	goObiect(T,R,Rez,X,Y).
goObiect(L,R):-
	goObiect(L,[],R,1000,1000).


%Ia primul si ultimul element
%%%%%%%%
getElemente([(O,C)|_],O,C).

%Ia primul numar dintr-o lista
getFirst([N|_],N).



