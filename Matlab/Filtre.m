fs = 25; % freqüència de mostreig
f0 = 55; % freqüència de notch 50 Hz 
f1=60; % frequencia de noch 60Hz
Q = 2; % factor de qualitat

w0 = (2*pi*f0)/fs; % freqüència angular
w1 = (2*pi*f1)/fs;
bw1=w1/Q;
bw = w0/Q; % ample de banda
a = fir1(200, (2*pi*0.5)/fs , 'high');
b = fir1(200, 0.8, "low");
d = a+b;
% obtenim la resposta en freqüència
[h, w] = freqz(d, 1, 1024, fs); 

% obtenim el vector de freqüències en Hz


% gràfic de la resposta en freqüència
plot((w/(pi)), 20*log10(abs(h))); % mostrem les freqüències en Hz

xlabel('Freqüència (Hz)');
ylabel('Amplitud (dB)');
title('Resposta en freqüència del filtre FIR');
grid on;

% Guardar els coeficients en un fitxer de text
fid = fopen('coeficients.txt', 'w');
fprintf(fid, '%f\n', d);
fclose(fid);

%%



fs = 25; % freqüència de mostreig
f1 = 0.5; % freqüència de tall inferior
f2 = 3; % freqüència de tall superior
Wn = [f1 f2]/fs; % freqüències normalitzades
b = fir1(200, [0.15 0.8], 'bandpass'); % obtenim els coeficients del filtre FIR

% obtenim la resposta en freqüència
[h, w] = freqz(b, 1, 1024, fs);

% gràfic de la resposta en freqüència
plot((w/(pi)), 20*log10(abs(h))); % mostrem les freqüències en Hz
xlabel('Freqüència (Hz)');
ylabel('Amplitud (dB)');
title('Resposta en freqüència del filtre passabanda FIR');
grid on;

% Guardar els coeficients en un fitxer de text
fid = fopen('coeficients.txt', 'w');
fprintf(fid, '%f\n', b);
fclose(fid);






