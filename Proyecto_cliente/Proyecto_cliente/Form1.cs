using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;

namespace Proyecto_cliente
{
    public partial class Form1 : Form
    {
        Socket server;
        string nom;
        string nom2;
        string pword;
        string rnom, rpword;
        bool finalizado = false;
        // int fila;
        // string invitacion;
        delegate void DelegadoParaEscribir(string text);
        delegate void DelegadoParaActualizarLista(string[] nombres, int num);
        delegate void DelegadoParaGroupBox();

        public void Setnom(string usuario)
        {
            rnom = usuario;
        }
        public void SetPassword(string contraseña)
        {
            rpword = contraseña;
        }

        public Form1()
        {
            InitializeComponent();
        }

        private void Conectar_Click(object sender, EventArgs e)
        {
            //Creamos un IPEndPoint con el ip del servidor y puerto del servidor 
            //al que deseamos conectarnos
            IPAddress direc = IPAddress.Parse("192.168.56.103");
            IPEndPoint ipep = new IPEndPoint(direc, 9070);


            //Creamos el socket 
            server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                server.Connect(ipep);//Intentamos conectar el socket
                this.BackColor = Color.Pink;
                MessageBox.Show("Conectado");

            }
            catch (SocketException)
            {
                //Si hay excepcion imprimimos error y salimos del programa con return 
                MessageBox.Show("No he podido conectar con el servidor");
                return;
            } 

        }

        private void Desconectar_Click(object sender, EventArgs e)
        {
            //Mensaje de desconexión
            string mensaje = "0/";

            byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);

            // Nos desconectamos
            this.BackColor = Color.Gray;
            server.Shutdown(SocketShutdown.Both);
            server.Close();
        }

        private void IniciarSesion_Click(object sender, EventArgs e)
        {
            //codigo 2 inciar sesion
            
           
                string mensaje = "2/" + Username.Text + "/" + Password.Text;
                // Enviamos al servidor el nombre tecleado convertido en un vector de bytes
                byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                server.Send(msg);

                //Recibimos la respuesta del servidor
                byte[] msg2 = new byte[500];
                server.Receive(msg2);
                mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];
               
                int registrado = Convert.ToInt32(mensaje);
                if (registrado == 1)
                {
                    MessageBox.Show("Usuario logeado correctamente");
                }
                else
                {
                    MessageBox.Show("Este usuario no esta registrado");
                    finalizado = true;
                }                    
        }

        private void Aceptar_Click(object sender, EventArgs e)
        {
            if (radioButton1.Checked) // CONSULTA 1. Jugador con mas victorias
            {
                
                string mensaje = "3/";
                // Enviamos al servidor la consulta a realizar
                byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje); // cojo el string y lo convierto a un vector de bytes
                server.Send(msg);

                //Recibimos la respuesta del servidor
                byte[] msg2 = new byte[80];
                server.Receive(msg2); // deja la respuesta
                mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0]; //convierto el vector en un string 
                MessageBox.Show("El jugador con mas victorias es: " + mensaje);
            }
            else if (MasLarga.Checked) //CONSULTA 2: Conseguir partida más larga. 
            {
                string mensaje = "4/";
                // Enviamos al servidor la consulta a realizar
                byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                server.Send(msg);

                //Recibimos la respuesta del servidor
                byte[] msg2 = new byte[80];
                server.Receive(msg2);
                mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0]; 
                MessageBox.Show("La partida más larga ha durado: " + mensaje);
            
            }
            else // CONSULTA 3. Partidas ganadas por el jugador introducido por teclado
            {
                string mensaje = "5/" + textBox1.Text ;
                // Enviamos al servidor la consulta a realizar con el nombre tecleado
                byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                server.Send(msg);

                //Recibimos la respuesta del servidor
                byte[] msg2 = new byte[80];
                server.Receive(msg2);
                mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0]; 
                MessageBox.Show(textBox1.Text + " ha ganado " + mensaje + " partidas");
                

            }
        }

        private void Registro_Click(object sender, EventArgs e)
        {
            // Envia el nombre y la constraseña del registro con el código 1 y separado por /
            string mensaje = "1/" + Convert.ToString(Username.Text) + "/" + Convert.ToString(Password.Text);
            byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);

            byte[] msg2 = new byte[80];
            server.Receive(msg2);
            mensaje = Encoding.ASCII.GetString(msg2).Split('/')[0];
            if (Convert.ToInt32(mensaje) == 1)
            {
                MessageBox.Show("Registrado correctamente");
            }
            else 
            { 
                MessageBox.Show("Error al registrarse"); 
            }
            //Limpiamos los cuadros de texto
            Username.Clear();
            Password.Clear();
        }

        


    }
}
