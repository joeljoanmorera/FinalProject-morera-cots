v=load('dades.txt');
v1=load('dades1.txt');
vmean=mean(v);
v=v-vmean;
v1mean=mean(v1);
v1=v1-v1mean;

figure;
subplot (2,1,1);
plot(v);
title('HeartBeat sense filtrar')
subplot(2,1,2)
plot(v1);
title('HeartBeat filtrat')

N1=length(v1);
N=length(v);
V=fft(v);
V1=fft(v1);
fs=25;

f = (0:N-1)*(fs/N);
f1= (0:N1-1)*(fs/N1);

figure;
subplot (2,1,1);
plot(f, abs(V));
xlabel('Freqüència (Hz)');

ylabel('No filtrat Mòdul');

subplot(2,1,2);
plot(f1, abs(V1));
xlabel('Frequencia (Hz)');
ylabel('Filtrat Modul');