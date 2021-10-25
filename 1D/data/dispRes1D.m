clear variables;
close all;
clc;

Fichier = 'Resultat/Inhomogene.txt';


fileID = fopen( Fichier, 'r' );         % Ouverture du fichier
taille = str2num( fgetl( fileID ) );    % Dimensions de l'image

reponseAir=zeros(taille(1,2),taille(1,1));
%prégénération de la matrice pour gangner du temps

espaceX=str2num(fgetl( fileID ) );
for i=1:1:taille(1,2);
tmp = str2num(fgetl( fileID ) );
t(1,i)=tmp(1,1);
reponseAir(i,:)=tmp(1,2:end)-273.15;
end
fclose(fileID);                       %% Fermeture du fichier

figure;
imagesc(espaceX,t,reponseAir);title('Fer                                Cuivre');
xlabel('Distance en metre');
ylabel('Temps en seconde');
cb=colorbar;
title(cb,'Temperature en °C')
