
fichier=['TypesDeMateriaux.txt'];


fileID = fopen( fichier, 'r' );         % Ouverture du fichier
taille = str2num( fgetl( fileID ) );    % Dimensions de l'image
X=taille(1,1);
Y=taille(1,2);

t=0;
affichage=zeros(Y,X);
%prégénération de la matrice pour gangner du temps


for i=1:Y
    affichage(i,:)=str2num(fgetl( fileID ));
end

fclose(fileID);   

imagesc(affichage);title('Materiaux');
