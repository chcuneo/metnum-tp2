%% Leer datos.
data = dlmread('../data/train.csv',',',1,0);
labels = data(:,1);
X = data(:,2:end);
ntrain = size(X,1);

%% Mostrar un digito cualquiera, por ejemplo el de la posicion 7 en la base
digit = X(7,:);
fprintf('%d ',digit)
colormap gray;
% Utilizamos reshape. Esta rutina, dado un vector, la pasa a una matriz de
% las dimensiones indicadas. La genera por columnas.
subplot(1,2,1); % Subplot sirve para generar varios graficos en una misma figura
image(reshape(digit,28,28));
title('Imagen original de reshape.');
% Trasponemos que retorna el reshape para ver bien el digito.
subplot(1,2,2);
image(reshape(digit,28,28)');
title('Imagen traspuesta con reshape.');

%% Probamos el imagesc. Esto puede servir para graficar los "autodigitos"
% Transformamos la imagen original para que tenga valores entre 0 y 1.
digit_aux = digit/255;
colormap gray;
% Primer grafico: imagen original
subplot(1,3,1);
image(reshape(digit,28,28));
% Segundo grafico: directamente graficamos digit_aux con image().
subplot(1,3,2);
image(reshape(digit_aux,28,28));
% Uh, no se ve nada.
% Tercer grafico: graficamos usando imagesc.
subplot(1,3,3);
imagesc(reshape(digit_aux,28,28));
% Ok, no quedo exactamente igual ya que trata de utilizar todo el colormap.
% De todos modos el resultado se entiende.

%% Ultimo ejemplo, cvpartition

n = 50; % Cantidad de muestras.
K = 5;  % Cuantas particiones.
C = cvpartition(n,'KFold',K); % En C tenemos las particiones.

% Mostramos las particiones.
for i = 1:K,
    % Graficamos la particion i. Basicamente, es un vector de n valores 0 o 1s, donde
    % un 1 indica que la muestra es parte de la base de training.
    fprintf('Particion %d\n',i);
    fprintf('%d ',C.training(i)');
    fprintf('\n');
end
