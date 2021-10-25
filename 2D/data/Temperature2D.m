function Temperature2D(nomMateriau)

fichierMateriau=['Resultat/',nomMateriau,'/',nomMateriau,'_',num2str(0),'.txt'];


fileID = fopen( fichierMateriau, 'r' );         % Ouverture du fichier
taille = str2num( fgetl( fileID ) );    % Dimensions de l'image
T=taille(1,3);
X=taille(1,1);
Y=taille(1,2);

t=0;
reponseMateriau=zeros(Y,X);
%prégénération de la matrice pour gangner du temps
espaceX=str2num(fgetl( fileID ) ); % Dimensions de l'image en X
espaceY=str2num(fgetl( fileID ) ); % Dimensions de l'image en Y


for i=1:Y
    reponseMateriau(i,:)=str2num(fgetl( fileID ) )-273.15;
end

fclose(fileID);   
cmax=max(reponseMateriau(:));
cmin=min(reponseMateriau(:));

imagesc(espaceX,espaceY,reponseMateriau);title([nomMateriau,' a t=',num2str(t),'s']);
xlabel('Distance en metre');ylabel('Distance en metre');axis image ;
caxis([cmin cmax]);
colorbar;
cb=colorbar;
title(cb,'Temperature en °C')

pause;


    for i=1:T
        fichierMateriau=['Resultat/',nomMateriau,'/',nomMateriau,'_',num2str(i),'.txt'];

        fileID = fopen( fichierMateriau, 'r' );         % Ouverture du fichier
        taille = str2num( fgetl( fileID ) );    % Dimensions de l'image

        t=taille(1,3);
        %prégénération de la matrice pour gangner du temps

        for j=1:Y 
            reponseMateriau(j,:)=str2num(fgetl( fileID ) ) - 273.15;
        end
        fclose(fileID);                       %% Fermeture du fichier
        pause(0); 

        imagesc(espaceX,espaceY,reponseMateriau);title([nomMateriau,' a t=',num2str(t),'s']);
        xlabel('Distance en metre');ylabel('Distance en metre'); axis image ;
        caxis([cmin cmax]);
        colorbar;
        cb=colorbar;
        title(cb,'Temperature en °C')
    end
    


end

