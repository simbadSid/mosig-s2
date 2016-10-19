#use to display the output of udptarget or tcptarget
#e.g.: udptarget -p 5000 -k | R -f display.R 

y.lim=c(0,900)
max.points=20

t2<-NULL; x11()

t2<-read.table("stdin", nrows=1)
while(1){
nc<-nrow(t2) ; 

if(nc-max.points<1){b<-1}else {b<-nc-max.points}
new.vec<-read.table("stdin", nrows=1)
t2<-rbind(t2[c(b:nc),],new.vec)


plot(t2, type="l", ylim=y.lim)
}
