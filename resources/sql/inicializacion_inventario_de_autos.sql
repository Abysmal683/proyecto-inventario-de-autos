create table if nor exists vehiculos(
vin text primary key check(length(vin) = 17),
marca text not null,
modelo text not null,
epoca integer check (epoca >=1886),
color text,
placa text unique not null,
propietario text not null,
foto blob,
estado text check(estado in ('Retirado','Almacenado'))not null
);
insert into vehiculos (vin,marca,modelo,epoca,color,placa,propietario,estado)
values ('1HGCM82633A004352','Honda','Civic',2020,'Negro metálico','ABC-123','Abyss','Almacenado');
select * from vehiculos;
create table if not exists datos_tecnicos(
id integer primary key autoincrement,
vin text not null,
motor text check (motor in ('Gasolina','Diesel','Eletrico','Hibrido')) not NULL,
kilometraje integer check (kilometraje >=0),
puertas integer check(puertas >= 0 and puertas <= 12),
carroceria text not null,
detalles_adicionales text,
foreign key (vin) references vehiculos (vin) on delete cascade 
);
select * FROM datos_tecnicos;